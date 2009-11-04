#include "ConstructionUnitGroup.h"

using namespace std;
using namespace springai;
using namespace brainSpace;

ConstructionUnitGroup::ConstructionUnitGroup( AIClasses* aiClasses, int groupID ) : BrainGroup( aiClasses, groupID )
{
	BaseDefenseCounter = 0;
	BaseDefenseCounterStart = 0; 
	figureId = 0;
	BaseDefenseHitBorder = false;
}

ConstructionUnitGroup::~ConstructionUnitGroup()
{
	ai->utility->Log(ALL, MISC, "Trying to remove graphics of figureId %i", figureId);
	ai->utility->RemoveGraphics(figureId);
}
///@return the UnitDef's of the units that the group is able to construct
vector<UnitDef*> ConstructionUnitGroup::IsAbleToBuild()
{
	set<UnitDef*> buildableUnits;
	vector<UnitDef*> returnVals;

	for ( map<Unit*, bool>::iterator it = Units.begin() ; it != Units.end() ; it++ )
	{
		vector<UnitDef*> units = it->first->GetDef()->GetBuildOptions();
		for (unsigned int j = 0 ; j < units.size() ; j++ )
		{
			buildableUnits.insert( units[j] );
		}
	}

	set<UnitDef*>::iterator iter;
	for ( iter = buildableUnits.begin() ; iter != buildableUnits.end() ; iter++ )
	{
		returnVals.push_back( *iter );
	}
	return returnVals;
}

bool ConstructionUnitGroup::IsAbleToBuild(UnitDef* unit)
{
	for ( map<Unit*, bool>::iterator it = Units.begin() ; it != Units.end() ; it++ )
	{
		vector<UnitDef*> units = it->first->GetDef()->GetBuildOptions();

		for (unsigned int j = 0 ; j < units.size() ; j++ )
		{
			if ( units[j] == unit ) {
				return true;
			}
		}
	}
	return false;
}

///Tells the group to complete the order given
void ConstructionUnitGroup::AssignBuildOrder( SBuildUnitCommand order )
{
	//SAIFloat3 buildPos = Units[0]->GetPos();
	//SAIFloat3 buildPos = Units.begin()->first->GetPos(); //ai->callback->GetMap()->GetStartPos();
	SAIFloat3 buildPos = ai->commander->GetPos();
	/*buildPos = ai->callback->GetMap()->GetStartPos();
	buildPos.z += (ai->utility->GetUnitDef("armlab")->GetZSize() * 8)/2
				+ (ai->utility->GetSolarDef()->GetZSize()*8)/2;*/
	
	UnitDef *unitDef = UnitDef::GetInstance(ai->callback, order.toBuildUnitDefId);

	ai->utility->Log(ALL, MISC, "order tobuild id: %d", order.toBuildUnitDefId );
	ai->utility->Log(ALL, MISC, "order options : %d", order.options );
	ai->utility->Log(ALL, MISC, "order timeout : %d", order.timeOut );
	ai->utility->Log(ALL, MISC, "order facing: %d", order.facing );
	
	Idle = false;
	
	order.unitId = ai->commander->GetUnitId();
	ai->utility->Log(ALL, MISC, "order builder id: %d", order.unitId );
	order.timeOut = 40000;


	if ( order.toBuildUnitDefId == ai->utility->GetMexDef()->GetUnitDefId() )
	{
		ai->utility->Log(ALL, MISC, "Find extractor build spot" );
		order.buildPos = FindClosestMetalExtractionSite( buildPos );
		order.buildPos.y = 30;
		ai->utility->Log(ALL, MISC, "Metal extractor build position set" );
	}
	//Check to see if unit-to-build is LLT, the defense structure, and find good spot
	else if (order.toBuildUnitDefId == ai->utility->GetLLTDef()->GetUnitDefId()) {
		ai->utility->Log(ALL, MISC, "Find defense build spot" );
		order.buildPos = FindBestDefensePosition(ai->utility->GetLLTDef(), buildPos);
		ai->utility->Log(ALL, MISC, "Defense build position set" );
	}
	else if (order.toBuildUnitDefId == ai->utility->GetSolarDef()->GetUnitDefId())
	{
		ai->utility->Log(ALL, MISC, "Find solar build spot" );
		order.buildPos = FindGoodBuildSite(/*this->GetPos()*/buildPos,unitDef, 1024);
		ai->utility->Log(ALL, MISC, "Solar build position set" );
	}
	else
	{
		ai->utility->Log(ALL, MISC, "Find lab build spot" );
		order.buildPos = FindClosestNonConflictingBuildSite(unitDef, buildPos, 1000, 0, 0);
		ai->utility->Log(ALL, MISC, "Lab build position set" );			
	}
	
	//ai->utility->DrawLine(Units.begin()->first->GetPos(), order.buildPos, true);

	//Did we get an errones buildPos`?
	if (order.buildPos.y == -1)
	{
		ai->utility->Log(ALL, MISC, "Could not get a build position in AssignBuildOrder()...");
		return;
	}

	if (figureId == 0)
		figureId = ai->utility->DrawCircle(order.buildPos, 50, figureId);
	else
		ai->utility->DrawCircle(order.buildPos, 50, figureId);

	/*//Can we build here without blocking ourself?
	if (BuildBlocksSelf(unitDef, order.buildPos, order.facing))
	{
		ai->utility->Log(ALL, MISC, "We would block ourself if we build this %s!", unitDef->GetName());
		return;
	}*/
	
	//TODO: Add the target action directly with HandleCommand, when we can, instead of using out own step-by-step pathfinding
	//ai->callback->GetEngine()->HandleCommand( 0, -1, COMMAND_UNIT_BUILD, &order );
	
	ai->utility->GoTo(order.unitId, order.buildPos);
	order.options = UNIT_COMMAND_OPTION_SHIFT_KEY;
	ai->callback->GetEngine()->HandleCommand( 0, -1, COMMAND_UNIT_BUILD, &order );
}

SAIFloat3 ConstructionUnitGroup::FindBestDefensePosition(UnitDef *unitDef, SAIFloat3 buildPos)
{
	ai->utility->ChatMsg("We are now building a Defense structure: %s", unitDef->GetName());
	//Divide map into quads, and find corner which buildPos is within
	int mapSplitX = (ai->callback->GetMap()->GetWidth() / 2) * 8;
	int mapSplitZ = (ai->callback->GetMap()->GetHeight() / 2) * 8;
	float baseX = buildPos.x;
	float baseZ = buildPos.z; 
	float deltaX, deltaZ;
	int defenseDensity = 3;
	float weaponRange = unitDef->GetMaxWeaponRange();
	/*
	1|2
	---
	3|4
	*/
	if ( baseX <= mapSplitX && baseZ <= mapSplitZ ) //Quad 1
	{
		//ai->utility->ChatMsg("Quad 1");
		deltaX = weaponRange - ( BaseDefenseCounter%2==0 ? ((BaseDefenseCounterStart+BaseDefenseCounter)/2)*(weaponRange/defenseDensity) : 0 );
		deltaZ = weaponRange - ( BaseDefenseCounter%2==1 ? ((BaseDefenseCounterStart+BaseDefenseCounter)/2+1)*(weaponRange/defenseDensity) : 0 );
	}
	else if ( baseX > mapSplitX && baseZ <= mapSplitZ ) //Quad 2
	{
		//ai->utility->ChatMsg("Quad 2");
		deltaX = -weaponRange + ( BaseDefenseCounter%2==0 ? ((BaseDefenseCounterStart+BaseDefenseCounter)/2)*(weaponRange/defenseDensity) : 0 );
		deltaZ = weaponRange - ( BaseDefenseCounter%2==1 ? ((BaseDefenseCounterStart+BaseDefenseCounter)/2+1)*(weaponRange/defenseDensity) : 0 );
	}
	else if ( baseX <= mapSplitX && baseZ > mapSplitZ ) //Quad 3
	{
		//ai->utility->ChatMsg("Quad 3");
		deltaX = weaponRange - ( BaseDefenseCounter%2==0 ? ((BaseDefenseCounterStart+BaseDefenseCounter)/2)*(weaponRange/defenseDensity) : 0 );
		deltaZ = -weaponRange + ( BaseDefenseCounter%2==1 ? ((BaseDefenseCounterStart+BaseDefenseCounter)/2+1)*(weaponRange/defenseDensity) : 0 );
	}
	else //Quad 4
	{
		//ai->utility->ChatMsg("Quad 4");
		deltaX = -weaponRange + ( BaseDefenseCounter%2==0 ? ((BaseDefenseCounterStart+BaseDefenseCounter)/2)*(weaponRange/defenseDensity) : 0 );
		deltaZ = -weaponRange + ( BaseDefenseCounter%2==1 ? ((BaseDefenseCounterStart+BaseDefenseCounter)/2+1)*(weaponRange/defenseDensity) : 0 );
	}
	//ai->utility->ChatMsg("WeaponRange: %f", weaponRange);
	//ai->utility->ChatMsg("deltX: %f", deltaX);
	buildPos.x += deltaX;
	buildPos.z += deltaZ;

	
	BaseDefenseCounter+=2;

	if (BaseDefenseCounter >= 4) 
	{
		if (BaseDefenseCounter%2==0)
		{
			BaseDefenseCounter = 1;
		}
		else
		{
			BaseDefenseCounter = 0;
			BaseDefenseCounterStart += 4;
		}

	}
	return buildPos;
}

///puts an order into a queue to be done later (when the group goes idle)
void ConstructionUnitGroup::QueueBuildOrder( SBuildUnitCommand order )
{
	ai->utility->ChatMsg( "Placing in queue" );
	BuildQueue.push( order );
	ai->utility->ChatMsg( "Placing in queue done" );
	ai->utility->ChatMsg( "Size of build queue: %d", BuildQueue.size() );
}

///Is the group idle, and has nothing in its queue?
bool ConstructionUnitGroup::IsIdle()
{
	return (Idle==true && BuildQueue.size()==0);
}
///for telling the group that it is now idle
void ConstructionUnitGroup::SetAvailable()
{
	ai->utility->Log(ALL, MISC, "ConstructionUnitGroup::SetAvailable()");
	Idle = true;
	if ( BuildQueue.size() > 0 )
	{
		const SBuildUnitCommand next = BuildQueue.front();
		BuildQueue.pop();
		ai->utility->ChatMsg( "Queue is now: %d units long", BuildQueue.size() );
		AssignBuildOrder( next );
	}
	
}

///@returns whether a new building would block a lab
bool ConstructionUnitGroup::BuildBlocksSelf(UnitDef *toBuildUnitDef, SAIFloat3 pos, int facing)
{
	ai->utility->Log(ALL, MISC, "BuildBlocksSelf check position (%f,%f)...", pos.x, pos.z);
	
	vector<Unit*> units = ai->callback->GetFriendlyUnits();
	SAIFloat3 fromPos;
	UnitDef *unitDef;
	//Check if the new building at the selected location would block any exsisting labs
	for (int i = 0; i < (int)units.size(); i++) 
	{
		unitDef = units[i]->GetDef();
		if ( strcmp(unitDef->GetName(), "armlab")==0 )
		{
			//Use pathfinding to check if unit-exit of the lab units[i] has a path out of the base
			//without using the locaion of the new building
			fromPos = GetUnitExitOfLab(units[i]->GetPos(), unitDef, units[i]->GetBuildingFacing());
			ai->utility->Log(ALL, MISC, "BuildBlocksSelf is checking an old lab...");
			//pos.y = 50.0;
			//ai->utility->DrawLine(pos, fromPos, true);
			//ai->utility->DrawLine(fromPos, GetSafePosition(), true);
			if (!ai->knowledge->mapInfo->pathfindingMap->IsPossibleToEscapeFrom(unitDef->GetBuildOptions()[0], toBuildUnitDef, pos, fromPos, ai->utility->GetSafePosition())) 
			{
				//There is a blocking problem with that build
				ai->utility->Log(ALL, MISC, "BuildBlocksSelf blocked build by reason 1 (No path from exit of an old %s)", unitDef->GetName());
				return true;
			}
		}
	}
	ai->utility->Log(ALL, MISC, "BuildBlocksSelf check 1 done");
	//If what we want to build is a lab, check that this position allows its units a path out of the base
	//without using the locaion of the new building
	if ( strcmp(toBuildUnitDef->GetName(), "armlab")==0) 
	{
		fromPos = GetUnitExitOfLab(pos, toBuildUnitDef, facing);
		if (!ai->knowledge->mapInfo->pathfindingMap->IsPossibleToEscapeFrom(toBuildUnitDef->GetBuildOptions()[0], toBuildUnitDef, pos, fromPos, ai->utility->GetSafePosition()))
		{
			ai->utility->Log(ALL, MISC, "BuildBlocksSelf blocked build by reason 2 (No path from exit of this new %s)", toBuildUnitDef->GetName());
			return true;
		}
	}
	ai->utility->Log(ALL, MISC, "BuildBlocksSelf check 2 done");
	//If we build this new building, does the commander have a path out of the base?
	fromPos = ai->commander->GetPos();
	///TODO: Maybe ensure that the commander does not walk into a building-block "trap" :)
	if (!ai->knowledge->mapInfo->pathfindingMap->IsPossibleToEscapeFrom(ai->commander->GetDef(), toBuildUnitDef, pos, fromPos, ai->utility->GetSafePosition()))
	{
		ai->utility->Log(ALL, MISC, "BuildBlocksSelf blocked build by reason 3 (No path for commander)");
		return true;
	}
	ai->utility->Log(ALL, MISC, "BuildBlocksSelf check 3 done");
	//ai->utility->Log(ALL, MISC, "Commander: %s", commander->GetDef()->GetName());

	if ( !ai->knowledge->mapInfo->pathfindingMap->PathExists(ai->commander->GetDef(), fromPos, pos) )
	{
		ai->utility->Log( ALL, MISC, "BuildBlocksSelf blocked build by reason 4 (No path to buildsite)");
		return true;
	}
	ai->utility->Log(ALL, MISC, "BuildBlocksSelf check 4 done");
	return false;
}

///@return unit-exit of a armlab or vehicleplant (bottom on facing=0)
SAIFloat3 ConstructionUnitGroup::GetUnitExitOfLab(SAIFloat3 centerPos, UnitDef *unitDef, int facing)
{
	SAIFloat3 pos = centerPos;
	switch (facing)
	{
		case 0:
			pos.z += (unitDef->GetZSize()/2)*8+8;break;
		case 1:
			pos.x -= (unitDef->GetZSize()/2)*8+8;break;
		case 2:
			pos.z -= (unitDef->GetZSize()/2)*8+8;break;
		case 3:
			pos.x += (unitDef->GetZSize()/2)*8+8;break;
		default:
			ai->utility->Log(CRITICAL, MISC, "GetUnitExitOfLab got unexpected facing!");
	}
	return pos;
}

///@returns whether the build location is on a metal extraction site
bool ConstructionUnitGroup::IsMetalExtracitonSite(UnitDef *unitDef, SAIFloat3 pos)
{
	if (ai->utility->IsMetalMap())
	{
		return false;//Since metal can be extracted from all spots, there are no limited metal extraction sites
	}
	SAIFloat3 closestMexSite = FindClosestMetalExtractionSite(pos, false);
	//pos.y = 50;
	//closestMexSite.y = 50;
	//ai->utility->DrawLine(pos, closestMexSite);
	return InersectsWithMex(unitDef, pos, closestMexSite); 
}

bool ConstructionUnitGroup::InersectsWithMex(UnitDef *unitDef, SAIFloat3 pos, SAIFloat3 mexPos)
{
	if (ai->utility->IsMetalMap())
	{
		return false;//Since metal can be extracted from all spots, there are no limited metal extraction sites
	}
	UnitDef *mexDef = ai->utility->GetMexDef();
	bool retVal = ai->math->BoxIntersect(	pos,
											unitDef->GetXSize()*8,
											unitDef->GetZSize()*8,
											mexPos,
											mexDef->GetXSize()*8,
											mexDef->GetZSize()*8);
	ai->utility->Log(ALL, MISC, "Mex intersect box 1 at (%f,%f,%f): %ix%i", pos.x, pos.y, pos.z, unitDef->GetXSize()*8, unitDef->GetZSize()*8);
	ai->utility->Log(ALL, MISC, "Mex intersect box 2 at (%f,%f,%f): %ix%i", mexPos.x, mexPos.y, mexPos.z, mexDef->GetXSize()*8, mexDef->GetZSize()*8);
	ai->utility->Log(ALL, MISC, "Checking InersectsWithMex: %i", retVal);
	return retVal;
}

///Find closest buildPos, allowing only metal extractors on metal spots, and ensures no blocking buildings
///@return the closest non-conflicting buildsite
SAIFloat3 ConstructionUnitGroup::FindClosestNonConflictingBuildSite(UnitDef *unitDef, SAIFloat3 buildPos, float searchRadius, int minDist, int facing)
{
	SAIFloat3 pos = (SAIFloat3){0,0,0};
	SAIFloat3 searchPos = buildPos;
	SAIFloat3 closestMexSite = (SAIFloat3){0,0,0};
	float tempMinRadius = 0;
	bool firstRun = true;
	short unsigned int corner = 0;


	ai->utility->Log(ALL, MISC, "FindClosestNonConflictingBuildSite... %d",unitDef->GetUnitDefId());

	ai->utility->ChatMsg("FindClosestNonConflictingBuildSite started");

	do
	{
		if (!firstRun) {
			if ( corner>3 )
			{
				tempMinRadius = tempMinRadius +
					ai->utility->EuclideanDistance(buildPos, pos) +
					unitDef->GetRadius();//Increase minimum distance a suitable amount
				corner = 0;
			}
		} else {
			firstRun = false;
		}
		searchPos = buildPos;
		switch (corner)
		{
			case 0:
				searchPos.z += tempMinRadius;
				break;
			case 1:
				searchPos.x -= tempMinRadius;
				break;
			case 2:
				searchPos.z -= tempMinRadius;
				break;
			case 3:
				searchPos.x += tempMinRadius;
				break;
		}
		corner++;
		pos = ai->callback->GetMap()->FindClosestBuildSite( *unitDef , searchPos, searchRadius, minDist, facing);
		
		if (!ai->utility->IsMetalMap()) {
			closestMexSite = FindClosestMetalExtractionSite(pos, false);
		}
		else
		{
			closestMexSite = pos;
		}
		ai->utility->Log(ALL, MISC, "Doing FindClosestNonConflictingBuildSite iteration");
		if (tempMinRadius>searchRadius) break;
	} while ( InersectsWithMex(unitDef, pos, closestMexSite)
			  ||
			  BuildBlocksSelf(unitDef, pos, facing) );
	
	ai->utility->ChatMsg("FindClosestNonConflictingBuildSite ended");

	if (ai->utility->EuclideanDistance(pos, buildPos) > searchRadius)
	{
		pos = buildPos; //We did not find a good place to build... Return orginal buildPos,
		pos.y = -1;		//and set y=-1 to indicate error
	}
	
	return pos;
}

///Maybe we should look into Voronoi diagrams to optimize this :P
///@return the closest metal spot to a given position
SAIFloat3 ConstructionUnitGroup::FindClosestMetalExtractionSite(SAIFloat3 pos, bool checkIfItBlocks/*, Resource* metal */ )
{
	ai->utility->Log(ALL, MISC, "FindClosestMetalExtractionSite...");
	UnitDef *mexDef = ai->utility->GetMexDef();
	if (ai->utility->IsMetalMap())
	{
		ai->utility->Log(ALL, MISC, "FindClosestMetalExtractionSite on MetalMap");
		//If this is an all-metal map, we can safely fallback to use FindClosestNonConflictingBuildSite
		return FindClosestNonConflictingBuildSite(mexDef, pos, 1000, 0, 0);
		//return ai->callback->GetMap()->FindClosestBuildSite( *mexDef, pos, 1000, 0, 0);
	}
	
	vector<SAIFloat3> spots;
	spots = ai->callback->GetMap()->GetResourceMapSpotsPositions( *(ai->utility->GetResource("Metal")), &pos );
	
	int numSpots = spots.size();
	int lowestIdx = -1;
	float closest = 9999999.9f;

	for ( int i  = 0 ; i < numSpots ; i++ )
	{
		//u->ChatMsg( "Metal spot: x: %f y: %f z: %f", spots[i].x, spots[i].y, spots[i].z );
		double distance = ai->utility->EuclideanDistance( spots[i], pos );
		//u->ChatMsg( "Distance: %f", distance );

		if ( distance < closest
			&& ai->callback->GetMap()->IsPossibleToBuildAt( *(ai->utility->GetMexDef()), spots[i], 0 )
			 && (!checkIfItBlocks || !BuildBlocksSelf(mexDef, spots[i], 0)) )
		{
			closest = distance;
			lowestIdx = i;
		}
	}
	///TODO: What if we didn't find an accaptable spot? (lowestIdx=-1)
	pos = spots[lowestIdx];
	
	return pos;

}

SAIFloat3 ConstructionUnitGroup::FindGoodBuildSite(SAIFloat3 builderPos, UnitDef* building, float radius)
{
	SAIFloat3 bestBuildSpot=builderPos;
	float bestDistance = radius*radius+1;
	bool foundBuildSite = false;
	
	vector<Unit*> nearByBuildings = ai->knowledge->selfInfo->baseInfo->GetUnitsInRange(builderPos, radius);
	vector<Unit*>::iterator it;
	for(it = nearByBuildings.begin(); it != nearByBuildings.end(); it++)
	{
		UnitDef* ud = (*it)->GetDef();
		if(!ud->IsBuilder())
		{
			SAIFloat3 unitPos = (*it)->GetPos();
			float muls[][2] = {{-1,0},{1,0},{0,-1},{0,1}};//left, right, up, down
			for(int i=0; i<4;i++)
			{
				SAIFloat3 newPos = unitPos;
				newPos.x += muls[i][0]*((ud->GetXSize()*8/2)+(building->GetXSize()*8/2));
				newPos.z += muls[i][1]*((ud->GetZSize()*8/2)+(building->GetZSize()*8/2));
				float newDist = ai->utility->EuclideanDistance(builderPos,newPos);
				
				if(newDist < bestDistance
					&&  ai->callback->GetMap()->IsPossibleToBuildAt(*building, newPos, 0)
					&& !IsMetalExtracitonSite(building, newPos)
					&& !BuildBlocksSelf(building, newPos, 0) )
				{
					//TODO: test if it is blocking a contruction yard
					bestBuildSpot = newPos;
					bestDistance = newDist;
					foundBuildSite = true;
				}
			}
		}
	}

	if (!foundBuildSite)
	{
		//We didn't find a good BuildSite (e.g. no non-builder buildings)
		bestBuildSpot = FindClosestNonConflictingBuildSite(building, builderPos, radius, 0, 0);
	}

	return bestBuildSpot;
}


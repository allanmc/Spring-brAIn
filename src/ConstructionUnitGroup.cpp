#include "ConstructionUnitGroup.h"

using namespace std;
using namespace springai;
using namespace brainSpace;

ConstructionUnitGroup::ConstructionUnitGroup( AIClasses* aiClasses, int groupID ) : BrainGroup( aiClasses, groupID )
{
	BaseDefenseCounter = 0;
	BaseDefenseCounterStart = 0; 
	BaseDefenseHitBorder = false;
	metalExtractorUnit = ai->utility->GetUnitDef("armmex");
}

ConstructionUnitGroup::~ConstructionUnitGroup()
{
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
	SAIFloat3 buildPos = commander->GetPos();
	UnitDef *unitDef = UnitDef::GetInstance(ai->callback, order.toBuildUnitDefId);

	ai->utility->ChatMsg( "order tobuild id: %d", order.toBuildUnitDefId );
	ai->utility->ChatMsg( "order options : %d", order.options );
	ai->utility->ChatMsg( "order timeout : %d", order.timeOut );
	ai->utility->ChatMsg( "order facing: %d", order.facing );
	
	
	Idle = false;
	
	order.unitId = Units.begin()->first->GetUnitId();
	ai->utility->ChatMsg( "order builder id: %d", order.unitId );
	order.timeOut = 40000;

	bool isMetalExtractor = false;
	bool isDefense = false;

	if ( order.toBuildUnitDefId == ai->utility->GetUnitDef("armmex")->GetUnitDefId() )
	{
		buildPos = FindClosestMetalExtractionSite( buildPos );
		isMetalExtractor = true;
		ai->utility->ChatMsg( "building metal extractor" );
	}
	

	//Check to see if unit-to-build is LLT, the defense structure, and find good spot
	if (order.toBuildUnitDefId == ai->utility->GetUnitDef("armllt")->GetUnitDefId()) {
		UnitDef* llt = ai->utility->GetUnitDef("armllt");
		isDefense = true;
		//ai->utility->ChatMsg("We are now building a Defense structure, LLT");
		//Divide map into quads, and find corner which buildPos is within
		int mapSplitX = (ai->callback->GetMap()->GetWidth() / 2) * 8;
		int mapSplitZ = (ai->callback->GetMap()->GetHeight() / 2) * 8;
		float baseX = buildPos.x;
		float baseZ = buildPos.z; 
		float deltaX, deltaZ;
		int defenseDensity = 3;
		float weaponRange = llt->GetMaxWeaponRange();
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
		
	}
	
	//Make sure that we can build at the desired position by finding the closest available buildsite to the desired site
	if ( !isMetalExtractor )
	{
		if(order.toBuildUnitDefId == ai->utility->GetUnitDef("armsolar")->GetUnitDefId())
		{
			order.buildPos = FindGoodBuildSite(this->GetPos(),unitDef, 1024);
			ai->utility->ChatMsg( "Solar build position set" );
		}
		else
		{
			ai->utility->ChatMsg( "Setting lab build position set" );	
			//order.buildPos = ai->callback->GetMap()->FindClosestBuildSite( *unitDef , buildPos, 200, 0, 0 );
			order.buildPos = FindClosestNonMetalExtractionSite(unitDef, buildPos, 1000, 0, 0);
			if (order.buildPos.y == -1)
			{
				ai->utility->ChatMsg("Could not FindClosestNonMexSite...");
				return;
			}
			ai->utility->ChatMsg( "Lab build position set" );			
		}
	}
	else
	{
		ai->utility->ChatMsg( "Metal extractor build position set" );
		order.buildPos = buildPos;
	}
	//ai->utility->DrawLine(Units.begin()->first->GetPos(), order.buildPos, true);

	//Can we build here without blocking ourself?
	if (BuildBlocksSelf(unitDef, order.buildPos, order.facing))
	{
		ai->utility->Log(ALL, MISC, "We would block ourself if we build this %s!", unitDef->GetName());
		return;
	}
	ai->callback->GetEngine()->HandleCommand( 0, -1, COMMAND_UNIT_BUILD, &order );
}

///puts an order into a queue to be done later (when the group goes idle)
void ConstructionUnitGroup::QueueBuildOrder( SBuildUnitCommand order )
{
	BuildQueue.push( order );
	//u->ChatMsg( "Size of build queue: %d", BuildQueue.size() );
}

///Is the group idle, and has nothing in its queue?
bool ConstructionUnitGroup::IsIdle()
{
	return (Idle==true && BuildQueue.size()==0);
}
///for telling the group that it is now idle
void ConstructionUnitGroup::SetAvailable()
{
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
	vector<Unit*> units = ai->callback->GetFriendlyUnits();
	SAIFloat3 fromPos;
	UnitDef *unitDef;
	//Check if the new building at the selected location would block any exsisting labs
	for (int i = 0; i < units.size(); i++) 
	{
		unitDef = units[i]->GetDef();
		if ( strcmp(unitDef->GetName(), "armlab")==0 )
		{
			//Use pathfinding to check if unit-exit of the lab units[i] has a path out of the base
			//without using the locaion of the new building
			fromPos = GetUnitExitOfLab(units[i]->GetPos(), unitDef, units[i]->GetBuildingFacing());
			if (false/*is there a safepath from fromPos?*/) 
			{
				//There is a blocking problem with that build
				ai->utility->Log(ALL, MISC, "BuildBlocksSelf blocked build by reason 1");
				return true;
			}
		}
	}
	//If what we want to build is a lab, check that this position allows its units a path out of the base
	//without using the locaion of the new building
	if ( strcmp(toBuildUnitDef->GetName(), "armlab")==0) 
	{
		fromPos = GetUnitExitOfLab(pos, toBuildUnitDef, facing);
		if (false/*is there a safepath from fromPos?*/)
		{
			ai->utility->Log(ALL, MISC, "BuildBlocksSelf blocked build by reason 2");
			return true;
		}
	}
	//If we build this new building, does the commander have a path out of the base?
	if (false/*is there a safepath from fromPos?*/)
	{
		ai->utility->Log(ALL, MISC, "BuildBlocksSelf blocked build by reason 3");
		return true;
	}
	return false;
}

///@return unit-exit of a armlab or vehicleplant (bottom on facing=0)
SAIFloat3 ConstructionUnitGroup::GetUnitExitOfLab(SAIFloat3 centerPos, UnitDef *unitDef, int facing)
{
	SAIFloat3 pos = centerPos;
	switch (facing)
	{
		case 0:
			pos.z += unitDef->GetZSize()/2;break;
		case 1:
			pos.x -= unitDef->GetZSize()/2;break;
		case 2:
			pos.z -= unitDef->GetZSize()/2;break;
		case 3:
			pos.x += unitDef->GetZSize()/2;break;
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
	SAIFloat3 closestMexSite = FindClosestMetalExtractionSite(pos);
	return InersectsWithMex(unitDef, pos, closestMexSite); 
}

bool ConstructionUnitGroup::InersectsWithMex(UnitDef *unitDef, SAIFloat3 pos, SAIFloat3 mexPos)
{
	UnitDef *mexDef = ai->utility->GetUnitDef("armmex");
	bool retVal = ai->math->BoxIntersect(	pos,
											unitDef->GetXSize(),
											unitDef->GetZSize(),
											mexPos,
											mexDef->GetXSize(),
											mexDef->GetZSize());
	ai->utility->Log(ALL, MISC, "Checking IsMetalExtracitonSite: %i", retVal);
	return retVal;
}

///Find closest buildPos which is not on a metal extraction site
///@return the closest nonmex buildsite
SAIFloat3 ConstructionUnitGroup::FindClosestNonMetalExtractionSite(UnitDef *unitDef, SAIFloat3 buildPos, float searchRadius, int minDist, int facing)
{
	SAIFloat3 pos;
	SAIFloat3 closestMexSite;
	float tempMinDist = minDist;
	bool firstRun = true;

	do
	{
		if (!firstRun) {
			tempMinDist = tempMinDist +
				ai->utility->EuclideanDistance(buildPos, closestMexSite) +
				unitDef->GetRadius();//Increase minimum distance a suitable amount
		} else {
			firstRun = false;
		}
		pos = ai->callback->GetMap()->FindClosestBuildSite( *unitDef , buildPos, searchRadius, tempMinDist, facing);
		closestMexSite = FindClosestMetalExtractionSite(pos);
		ai->utility->Log(ALL, MISC, "Doing FindClosestMetalExtractionSite iteration");
	} while ( !ai->utility->IsMetalMap()
			  &&
			  InersectsWithMex(unitDef, pos, closestMexSite)
			  &&
			  tempMinDist<searchRadius );
	
	if (ai->utility->EuclideanDistance(pos, buildPos) > searchRadius)
	{
		pos = buildPos; //We did not find a good place to build... Return orginal buildPos,
		pos.y = -1;		//and set y=-1 to indicate error
	}
	
	return pos;
}

///Maybe we should look into Voronoi diagrams to optimize this :P
///@return the closest metal spot to a given position
SAIFloat3 ConstructionUnitGroup::FindClosestMetalExtractionSite(SAIFloat3 pos/*, Resource* metal */ )
{
	if (ai->utility->IsMetalMap())
	{
		//ai->utility->Log(ALL, MISC, "FindClosestMetalExtractionSite on MetalMap");
		UnitDef *mexDef = ai->utility->GetUnitDef("armmex");
		//ai->utility->Log(ALL, MISC, "FindClosestMetalExtractionSite found  mexDef");
		return ai->callback->GetMap()->FindClosestBuildSite( *mexDef, pos, 1000, 0, 0);
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

		if ( distance < closest && ai->callback->GetMap()->IsPossibleToBuildAt( *metalExtractorUnit, spots[i], 0 ))
		{
			closest = distance;
			lowestIdx = i;
		}
	}

	pos = spots[lowestIdx];

	return pos;

}

SAIFloat3 ConstructionUnitGroup::FindGoodBuildSite(SAIFloat3 builderPos, UnitDef* building, float radius)
{
	SAIFloat3 bestBuildSpot=builderPos;
	float bestDistance = radius*radius+1;

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
					&& !IsMetalExtracitonSite(building, newPos))
				{
					//TODO: test if it is blocking a contruction yard
					bestBuildSpot = newPos;
					bestDistance = newDist;
				}
			}
		}
	}
	return bestBuildSpot;
}


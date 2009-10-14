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
		for ( int j = 0 ; j < units.size() ; j++ )
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

		for ( int j = 0 ; j < units.size() ; j++ )
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
	SAIFloat3 buildPos = ai->callback->GetMap()->GetStartPos();
	
	Idle = false;
	
	order.unitId = Units.begin()->first->GetUnitId();
	order.timeOut = 40000;
	
	vector<UnitDef*> defs = ai->callback->GetUnitDefs();


	bool isMetalExtractor = false;
	bool isDefense = false;
	int index = -1;
	//Check if the unit to build extracts metal. Set the buildpos accordingly.
	/*
	for ( int i = 0 ; i < defs.size() ; i++ )
	{
		if ( defs[i]->GetUnitDefId() == order.toBuildUnitDefId )
		{
			index = i;
			if ( strcmp( defs[i]->GetName(), "armmex" ) == 0 )
			{
				metalExtractorUnit = defs[i];	
				buildPos = FindClosestMetalExtractionSite( Units[0]->GetPos() );
				isMetalExtractor = true;
				ai->utility->ChatMsg( "MetalExtractor: %s", metalExtractorUnit->GetHumanName() );
				break;
			}
		}
	}*/
	if ( order.toBuildUnitDefId == ai->utility->GetUnitDef("armmex")->GetUnitDefId() )
	{
		buildPos = FindClosestMetalExtractionSite( buildPos );
		isMetalExtractor = true;
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
		UnitDef *unitDef = UnitDef::GetInstance(ai->callback, order.toBuildUnitDefId);
		order.buildPos = ai->callback->GetMap()->FindClosestBuildSite( *unitDef , buildPos, 200, 0, 0 );
	}
	else
	{
		//ai->utility->ChatMsg( "Metal extractor build position set" );
		order.buildPos = buildPos;
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
		//u->ChatMsg( "Queue is now: %d units long", BuildQueue.size() );
		AssignBuildOrder( next );
	}
	
}

///Maybe we should look into Voronoi diagrams to optimize this :P
///@return the closest metal spot to a given position
SAIFloat3 ConstructionUnitGroup::FindClosestMetalExtractionSite(SAIFloat3 pos/*, Resource* metal */ )
{
	vector<SAIFloat3> spots;
	
	//Get the metal resource.
	for ( int i = 0 ; i < ai->callback->GetResources().size() ;i++ )
	{
		if ( strcmp( ai->callback->GetResources()[i]->GetName(), "Metal" ) == 0 )
		{
			struct SAIFloat3 dummy;
			spots = ai->callback->GetMap()->GetResourceMapSpotsPositions( *ai->callback->GetResources()[i], &dummy );
		}
	}

	int numSpots = spots.size();
	int lowestIdx = -1;
	float closest = 9999999.9f;

	/*
	u->ChatMsg( "Unit name: %s - team %d", Units[0]->GetDef()->GetHumanName(), Units[0]->GetTeam() );
	u->ChatMsg( "Unit pos: x: %f y: %f z: %f", pos.x, pos.y, pos.z );
	u->ChatMsg( "%d metal spots", numSpots );
	*/
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

	//MetalMap* m = new MetalMap(Callback);
	pos = spots[lowestIdx];


	//u->ChatMsg( "Found spot: x: %f y: %f z: %f", pos.x, pos.y, pos.z );

	/*
	float searchRadius = 100.0f;
	float bestExtraction = 0.0f;

	//Find the spot that gives the most metal inside a small radius of the already found metal spot
	for ( int i = 0 ; i < numSpots ; i++ )
	{
		double distance = sqrt( pow( fabs( spots[i].x - pos.x ), 2 ) + pow( fabs( pos.z - spots[i].z ), 2  ) );
		if ( distance < searchRadius && spots[i].y > pos.y )
		{
			pos = Callback->GetMap()->FindClosestBuildSite( *metalExtractorUnit, spots[i], 16, 0, 0 );
			bestExtraction = spots[i].y;
		}
	}
	u->ChatMsg( "Improved spot: x: %f y: %f z: %f", pos.x, pos.y, pos.z );
	*/
	return pos;

}

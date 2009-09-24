#include "ConstructionUnitGroup.h"

UnitDef* metalExtractorUnit = NULL;

ConstructionUnitGroup::ConstructionUnitGroup( AIClasses* aiClasses ) : BrainGroup( aiClasses )
{
	BaseDefenseCounter = 0;
	BaseDefenseCounterStart = 0; 
	BaseDefenseHitBorder = false;
}

ConstructionUnitGroup::~ConstructionUnitGroup()
{
}


vector<UnitDef*> ConstructionUnitGroup::IsAbleToBuild()
{
	set<UnitDef*> buildableUnits;
	vector<UnitDef*> returnVals;

	for ( int i = 0 ; i < Units.size() ; i ++ )
	{
		vector<UnitDef*> units = Units[i]->GetDef()->GetBuildOptions();

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

bool ConstructionUnitGroup::IsAbleToBuild(UnitDef* unit) {
	for ( int i = 0 ; i < Units.size() ; i ++ )
	{
		vector<UnitDef*> units = Units[i]->GetDef()->GetBuildOptions();

		for ( int j = 0 ; j < units.size() ; j++ )
		{
			if ( units[j] == unit ) {
				return true;
			}
		}
	}
	return false;
}

void ConstructionUnitGroup::AssignBuildOrder( SBuildUnitCommand order )
{
	//SAIFloat3 buildPos = Units[0]->GetPos();
	SAIFloat3 buildPos = AI->Callback->GetMap()->GetStartPos();
	
	Idle = false;
	
	order.unitId = Units[0]->GetUnitId();
	order.timeOut = 40000;
	
	vector<UnitDef*> defs = AI->Callback->GetUnitDefs();


	bool isMetalExtractor = false;
	bool isDefense = false;
	int index = -1;
	//Check if the unit to build extracts metal. Set the buildpos accordingly.
	for ( int i = 0 ; i < defs.size() ; i++ )
	{
		if ( defs[i]->GetUnitDefId() == order.toBuildUnitDefId )
		{
			index = i;
			if ( strcmp( defs[i]->GetName(), "armmex" ) == 0 )
			{
				metalExtractorUnit = defs[i];	
				buildPos = FindClosestMetalExtractionSite( Units[0]->GetPos() /*, metal.at(0) */ );
				isMetalExtractor = true;
				AI->Utility->ChatMsg( "MetalExtractor: %s", metalExtractorUnit->GetHumanName() );
				break;
			}
		}
	}

	//Check to see if unit-to-build is LLT, the defense structure, and find good spot
	if (order.toBuildUnitDefId == AI->Utility->GetUnitDef("armllt")->GetUnitDefId()) {
		UnitDef* llt = AI->Utility->GetUnitDef("armllt");
		isDefense = true;
		AI->Utility->ChatMsg("We are now building a Defense structure, LLT");
		//Divide map into quads, and find corner which buildPos is within
		int mapSplitX = (AI->Callback->GetMap()->GetWidth() / 2) * 8;
		int mapSplitZ = (AI->Callback->GetMap()->GetHeight() / 2) * 8;
		int baseX = buildPos.x;
		int baseZ = buildPos.z; 
		int deltaX, deltaZ;
		int defenseDensity = 3;
		float weaponRange = llt->GetMaxWeaponRange();
		/*
		1|2
		---
		3|4
		*/
		if ( baseX <= mapSplitX && baseZ <= mapSplitZ ) //Quad 1
		{
			AI->Utility->ChatMsg("Quad 1");
			deltaX = weaponRange - ( BaseDefenseCounter%2==0 ? ((BaseDefenseCounterStart+BaseDefenseCounter)/2)*(weaponRange/defenseDensity) : 0 );
			deltaZ = weaponRange - ( BaseDefenseCounter%2==1 ? ((BaseDefenseCounterStart+BaseDefenseCounter)/2+1)*(weaponRange/defenseDensity) : 0 );
		}
		else if ( baseX > mapSplitX && baseZ <= mapSplitZ ) //Quad 2
		{
			AI->Utility->ChatMsg("Quad 2");
			deltaX = -weaponRange + ( BaseDefenseCounter%2==0 ? ((BaseDefenseCounterStart+BaseDefenseCounter)/2)*(weaponRange/defenseDensity) : 0 );
			deltaZ = weaponRange - ( BaseDefenseCounter%2==1 ? ((BaseDefenseCounterStart+BaseDefenseCounter)/2+1)*(weaponRange/defenseDensity) : 0 );
		}
		else if ( baseX <= mapSplitX && baseZ > mapSplitZ ) //Quad 3
		{
			AI->Utility->ChatMsg("Quad 3");
			deltaX = weaponRange - ( BaseDefenseCounter%2==0 ? ((BaseDefenseCounterStart+BaseDefenseCounter)/2)*(weaponRange/defenseDensity) : 0 );
			deltaZ = -weaponRange + ( BaseDefenseCounter%2==1 ? ((BaseDefenseCounterStart+BaseDefenseCounter)/2+1)*(weaponRange/defenseDensity) : 0 );
		}
		else //Quad 4
		{
			AI->Utility->ChatMsg("Quad 4");
			deltaX = -weaponRange + ( BaseDefenseCounter%2==0 ? ((BaseDefenseCounterStart+BaseDefenseCounter)/2)*(weaponRange/defenseDensity) : 0 );
			deltaZ = -weaponRange + ( BaseDefenseCounter%2==1 ? ((BaseDefenseCounterStart+BaseDefenseCounter)/2+1)*(weaponRange/defenseDensity) : 0 );
		}
		AI->Utility->ChatMsg("WeaponRange: %f", weaponRange);
		AI->Utility->ChatMsg("deltX: %f", deltaX);
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
		/*
		if ( (BaseDefenseCounter % 2 == 0 && (buildPos.x<0.0 || buildPos.x>mapSplitX*2)) 
			 ||
			 (BaseDefenseCounter % 2 == 1 && (buildPos.z<0.0 || buildPos.z>mapSplitZ*2))
			)
		{
			if (BaseDefenseHitBorder)
			{
				buildPos = Callback->GetMap()->GetStartPos();
			}
			else
			{
				BaseDefenseHitBorder = true;
				BaseDefenseCounter+=2;
			}
		}*/
		
		/*
		if ( BaseDefenseCounter % 2 == 0 && (buildPos.x<0.0 || buildPos.x>mapSplitX*2) )
		{
			BaseDefenseCounter=1;
		}
		else if ( BaseDefenseCounter % 2 == 1 && (buildPos.z<0.0 || buildPos.z>mapSplitZ*2) )
		{
			buildPos = Callback->GetMap()->GetStartPos();
		}*/
		
	}
	
	//Make sure that we can build at the desired position by finding the closest available buildsite to the desired site
	if ( !isMetalExtractor )
		order.buildPos = AI->Callback->GetMap()->FindClosestBuildSite( *defs[index] , buildPos, 200, 0, 0 );
	else
	{
		AI->Utility->ChatMsg( "Metal extractor build position set" );
		order.buildPos = buildPos;
	}
	AI->Callback->GetEngine()->HandleCommand( 0, -1, COMMAND_UNIT_BUILD, &order );
}

void ConstructionUnitGroup::QueueBuildOrder( SBuildUnitCommand order )
{
	BuildQueue.push( order );
	//u->ChatMsg( "Size of build queue: %d", BuildQueue.size() );
}

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

SAIFloat3 ConstructionUnitGroup::FindClosestMetalExtractionSite(SAIFloat3 pos/*, Resource* metal */ )
{
	vector<SAIFloat3> spots;
	//Get the metal resource.
	for ( int i = 0 ; i < AI->Callback->GetResources().size() ;i++ )
	{
		if ( strcmp( AI->Callback->GetResources()[i]->GetName(), "Metal" ) == 0 )
		{
			struct SAIFloat3 dummy;
			spots = AI->Callback->GetMap()->GetResourceMapSpotsPositions( *AI->Callback->GetResources()[i], &dummy );
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
		double distance = sqrt( pow( fabs( spots[i].x - pos.x ), 2 ) + pow( fabs( spots[i].z - pos.z ), 2  ) );
		//u->ChatMsg( "Distance: %f", distance );

		if ( distance < closest && AI->Callback->GetMap()->IsPossibleToBuildAt( *metalExtractorUnit, spots[i], 0 ))
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

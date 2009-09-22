#include "ConstructionUnitGroup.h"

UnitDef* metalExtractorUnit = NULL;

ConstructionUnitGroup::ConstructionUnitGroup( AICallback* callback ) : BrainGroup( callback )
{
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
	Utility* u = new Utility( Callback );
	SAIFloat3 buildPos = Units[0]->GetPos();

	Idle = false;
	
	order.unitId = Units[0]->GetUnitId();
	order.timeOut = 40000;
	
	vector<UnitDef*> defs = Callback->GetUnitDefs();
	vector<Resource*> metal;

	//Get the metal resource.
	for ( int i = 0 ; i < Callback->GetResources().size() ;i++ )
	{
		if ( strcmp( Callback->GetResources()[i]->GetName(), "Metal" ) == 0 )
		{
			metal.push_back( Callback->GetResources()[i] );
		}
	}

	bool isMetalExtractor = false;
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
				buildPos = FindClosestMetalExtractionSite( Units[0]->GetPos(), metal.at(0) );
				isMetalExtractor = true;
				break;
			}
		}
	}
	
	//Make sure that we can build at the desired position by finding the closest available buildsite to the desired site
	if ( !isMetalExtractor )
		order.buildPos = Callback->GetMap()->FindClosestBuildSite( *defs[index] , buildPos, 200, 0, 0 );
	else
		order.buildPos = buildPos;
	Callback->GetEngine()->HandleCommand( 0, -1, COMMAND_UNIT_BUILD, &order );
}

void ConstructionUnitGroup::QueueBuildOrder( SBuildUnitCommand order )
{
	Utility* u = new Utility( Callback );
	BuildQueue.push( order );
	u->ChatMsg( "Size of build queue: %d", BuildQueue.size() );
}

void ConstructionUnitGroup::SetAvailable()
{
	Idle = true;
	Utility* u = new Utility( Callback );
	if ( BuildQueue.size() > 0 )
	{
		const SBuildUnitCommand next = BuildQueue.front();
		BuildQueue.pop();
		//u->ChatMsg( "Queue is now: %d units long", BuildQueue.size() );
		AssignBuildOrder( next );
	}
}

SAIFloat3 ConstructionUnitGroup::FindClosestMetalExtractionSite(SAIFloat3 pos, Resource* metal )
{
	Utility* u = new Utility( Callback );
	struct SAIFloat3 dummy;
	vector<SAIFloat3> spots = Callback->GetMap()->GetResourceMapSpotsPositions( *metal, &dummy );
	int numSpots = spots.size();
	int lowestIdx = -1;
	float closest = 9999999.9f;

	for ( int i  = 0 ; i < numSpots ; i++ )
	{
		double distance = sqrt( pow( fabs( spots[i].x - pos.x ), 2 ) + pow( fabs( spots[i].z - pos.z ), 2  ) );
		if ( distance < closest && Callback->GetMap()->IsPossibleToBuildAt( *metalExtractorUnit, spots[i], 0 ))
		{
			closest = distance;
			lowestIdx = i;
		}
	}

	pos = spots[lowestIdx];
	float searchRadius = 20.0f;
	float bestExtraction = 0.0f;

	//Find the spot that gives the most metal inside a small radius of the already found metal spot
	for ( int i = 0 ; i < numSpots ; i++ )
	{
		double distance = sqrt( pow( fabs( spots[i].x - pos.x ), 2 ) + pow( fabs( pos.z - spots[i].z ), 2  ) );
		if ( distance < searchRadius && spots[i].y > pos.y )
		{
			pos = Callback->GetMap()->FindClosestBuildSite( *metalExtractorUnit, spots[i], 16, 2, 0 );
			bestExtraction = spots[i].y;
		}
	}
	return pos;

}
#include "ConstructionUnitGroup.h"

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
	Idle = false;
	order.unitId = Units[0]->GetUnitId();
	vector<UnitDef*> defs = Callback->GetUnitDefs();
	vector<Resource*> metal;

	//Get the metal resource.
	for ( int i = 0 ; i < Callback->GetResources().size() ;i++ )
	{
		if ( strcmp( Callback->GetResources()[i]->GetName(), "Metal" ) )
		{
			metal.push_back( Callback->GetResources()[i] );
			u->ChatMsg( "Metal found" );
		}
	}

	bool metalExtractor = false;
	int index = -1;
	//Check if the unit to build extracts metal. Set the buildpos accordingly.
	for ( int i = 0 ; i < defs.size() ; i++ )
	{
		if ( defs[i]->GetUnitDefId() == order.toBuildUnitDefId )
		{
			index = i;
			if ( defs[i]->GetExtractsResource( *metal.at(0) ))
			{
				u->ChatMsg( "Building extracts metal" );
				order.buildPos = FindClosestMetalExtractionSite( order.buildPos, *metal.at(0) );
				metalExtractor = true;
				break;
			}
		}
	}
	if ( !metalExtractor )
		order.buildPos = Callback->GetMap()->FindClosestBuildSite( *defs[index] , Units[0]->GetPos(), 200, 20, 0 );

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
		u->ChatMsg( "Queue is now: %d units long", BuildQueue.size() );
		AssignBuildOrder( next );
	}
}

SAIFloat3 ConstructionUnitGroup::FindClosestMetalExtractionSite(SAIFloat3 pos, Resource metal )
{

	struct SAIFloat3 dummy;
	vector<SAIFloat3> spots = Callback->GetMap()->GetResourceMapSpotsPositions( metal, &dummy );
	int numSpots = spots.size();
	float closest = 9999999.9f;
	int lowestIdx = -1;

	for ( int j  = 0 ; j < numSpots ; j++ )
	{
		double distance = sqrt( pow( fabs( spots[j].x - pos.x ), 2 ) + pow( fabs( pos.z - spots[j].z ), 2  ) );
		if ( distance < closest )
		{
			closest = distance;
			lowestIdx = j;
		}
	}

	float searchRadius = 50.0f;
	float bestExtraction = 0.0f;
	int finalLowestIdx = -1;
	return spots[lowestIdx];
	/*
	if ( lowestIdx != -1 )
	{
	for ( int i = 0 ; i < numSpots ; i++ )
	{
	if ( i != lowestIdx )
	{
	double distance = sqrt( pow( fabs( spots[i].x - spots[lowestIdx].x ), 2 ) + pow( fabs( spots[lowestIdx].z - spots[i].z ), 2  ) );
	if ( distance < searchRadius && spots[i].y > bestExtraction )
	{
	bestExtraction = spots[i].y;
	finalLowestIdx = i;
	}
	}
	}
	}
	return spots[finalLowestIdx];
	*/

}
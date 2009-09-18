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
	Idle = false;
	order.unitId = Units[0]->GetUnitId();
	order.buildPos = Units[0]->GetPos();
	vector<UnitDef*> u = Callback->GetUnitDefs();
	Callback->GetEngine()->HandleCommand( 0, -1, COMMAND_UNIT_BUILD, &order );
}

void ConstructionUnitGroup::QueueBuildOrder( SBuildUnitCommand order )
{
	Utility* u = new Utility( Callback );
	BuildQueue.push_back( order );
	u->ChatMsg( "Size of build queue: %d", BuildQueue.size() );
}

void ConstructionUnitGroup::SetAvailable( bool b )
{
	Idle = b;
	Utility* u = new Utility( Callback );
	u->ChatMsg( "Unit gone idle!!" );
	if ( BuildQueue.size() > 0 )
	{
		u->ChatMsg( "Build queue was not empty" );
		SBuildUnitCommand next = BuildQueue.at(0);
		for ( int i = 0 ; i < Callback->GetUnitDefs().size() ; i++ )
		{
			if ( Callback->GetUnitDefs()[i]->GetUnitDefId() == next.toBuildUnitDefId )
			{
				next.buildPos = Callback->GetMap()->FindClosestBuildSite( *Callback->GetUnitDefs()[i] , next.buildPos, 50, 20, 0 );
			}
		}
		Callback->GetEngine()->HandleCommand( 0, -1, COMMAND_UNIT_BUILD, &next );
	}
}
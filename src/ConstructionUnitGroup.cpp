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
	Utility* u = Utility::GetInstance( Callback );
	BuildQueue.push_back( order );
	u->ChatMsg( "Size of build queue: %d", BuildQueue.size() );
}
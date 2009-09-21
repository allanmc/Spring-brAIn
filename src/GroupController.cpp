#include "GroupController.h"


GroupController::GroupController( AICallback* callback )
{
	Callback = callback;
	ConstructionGroupMgr = new ConstructionGroupManager( callback );
	MilitaryGroupMgr = new MilitaryGroupManager( callback );
}

brainSpace::GroupController::~GroupController(void)
{
}

void GroupController::AddUnit(Unit *unit)
{
	if ( unit->GetDef()->IsBuilder() )
	{
		ConstructionGroupMgr->AddUnit( unit );
	}
	else if ( unit->GetDef()->IsAbleToAttack() )
	{
		MilitaryGroupMgr->AddUnit( unit );
	}

}

void GroupController::RemoveUnit(Unit *unit)
{
	if ( unit->GetDef()->IsBuilder() )
	{
		ConstructionGroupMgr->RemoveUnit( unit );
	}
	else if ( unit->GetDef()->IsAbleToAttack() )
	{
		MilitaryGroupMgr->RemoveUnit( unit );
	}
}

int GroupController::ErectBuilding(SBuildUnitCommand order)
{
	return ConstructionGroupMgr->DelegateBuildOrder( order );
}

void GroupController::UnitIdle( Unit* unit )
{
	if ( unit->GetDef()->IsBuilder() )
	{
		ConstructionGroupMgr->UnitIdle( unit );
	}
}
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
	else
	{
		MilitaryGroupMgr->UnitIdle(unit);
	}
}

void GroupController::AttackWithGroup(int enemy)
{
	if(MilitaryGroupMgr->GetNonAttackingGroups().size() > 0)
	{
		MilitaryGroupMgr->GiveAttackOrder(MilitaryGroupMgr->GetNonAttackingGroups()[0], enemy);
	}
}

void GroupController::ScoutWithIdleGroup()
{
	if(!MilitaryGroupMgr->GetIdleGroups().empty())
	{
		MilitaryGroupMgr->GiveScoutOrder(MilitaryGroupMgr->GetIdleGroups()[0]);
	}
}

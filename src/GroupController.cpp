#include "GroupController.h"
#include <stdlib.h>



GroupController::GroupController( AICallback* callback )
{
	Callback = callback;
	ConstructionGroupMgr = new ConstructionGroupManager( callback );
	MilitaryGroupMgr = new MilitaryGroupManager( callback );
	srand(0);
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
	if(MilitaryGroupMgr->GetIdleGroups().size() > 0)
	{
		MilitaryGroupMgr->GetIdleGroups()[0]->Attack(enemy);
	}
}

void GroupController::ScoutWithIdleGroup()
{
	int h = Callback->GetMap()->GetHeight();
	int w = Callback->GetMap()->GetHeight();
	SAIFloat3 pos;
	pos.x = rand() % w;
	pos.z = rand() % h;
	pos.y = 0;
	if(MilitaryGroupMgr->GetIdleGroups().size() > 0)
		MilitaryGroupMgr->GetIdleGroups()[0]->Scout(pos);
}
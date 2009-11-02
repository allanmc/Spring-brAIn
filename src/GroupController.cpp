#include "GroupController.h"

using namespace std;
using namespace springai;
using namespace brainSpace;

GroupController::GroupController( AIClasses* aiClasses )
{
	ai = aiClasses;
	ConstructionGroupMgr = new ConstructionGroupManager( ai );
	MilitaryGroupMgr = new MilitaryGroupManager( ai );
	
}

brainSpace::GroupController::~GroupController(void)
{
	delete ConstructionGroupMgr;
	ConstructionGroupMgr = NULL;
	delete MilitaryGroupMgr;
	MilitaryGroupMgr = NULL;
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

///finds a group capeble of constructing the building
///@return 1 if an idle group was found, or 0 if the order was queued
int GroupController::ErectBuilding(SBuildUnitCommand order)
{
	return ConstructionGroupMgr->DelegateBuildOrder( order );
}

bool GroupController::ConstructionGroupIsIdle()
{
	return ConstructionGroupMgr->IsIdle();
}

void GroupController::UnitIdle( Unit* unit )
{
	ai->utility->Log(ALL, MISC, "GroupController::UnitIdle()");
	if ( unit->GetDef()->IsBuilder() )
	{
		ConstructionGroupMgr->UnitIdle( unit );
	}
	else
	{
		MilitaryGroupMgr->UnitIdle(unit);
	}
}

///finds an idle group to attack the target.
void GroupController::AttackWithGroup(int enemy)
{
	if(MilitaryGroupMgr->GetNonAttackingGroups().size() > 0)
	{
		MilitaryGroupMgr->GiveAttackOrder(MilitaryGroupMgr->GetNonAttackingGroups()[0], enemy);
	}
}

///finds an idle group to scout with
void GroupController::ScoutWithIdleGroup()
{
	if(!MilitaryGroupMgr->GetIdleGroups().empty())
	{
		MilitaryGroupMgr->GiveScoutOrder(MilitaryGroupMgr->GetIdleGroups()[0]);
	}
}

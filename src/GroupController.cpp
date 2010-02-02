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
	ai->utility->Log(ALL, MISC, "ErectBuilding()...");
	return ConstructionGroupMgr->DelegateBuildOrder( order );
}

bool GroupController::ConstructionGroupIsIdle()
{
	return ConstructionGroupMgr->IsIdle();
}

void GroupController::UnitIdle( Unit* unit )
{
	ai->utility->Log(ALL, MISC, "GroupController::UnitIdle()");
	UnitDef *def = unit->GetDef();
	if ( def->IsBuilder() )
	{
		ConstructionGroupMgr->UnitIdle( unit );
	}
	else
	{
		MilitaryGroupMgr->UnitIdle(unit);
	}
	delete def;
}

///finds an idle group to attack the target.
void GroupController::AttackWithGroup(int enemy)
{
	ai->utility->Log(ALL, MISC, "AttackWithGroup::IsAttackGroupAvailable()");
	if(MilitaryGroupMgr->IsAttackGroupAvailable())
	{
		ai->utility->Log(ALL, MISC, "AttackWithGroup::IsAttackGroupAvailable() TRUE");
		MilitaryGroupMgr->GiveAttackOrder(MilitaryGroupMgr->GetIdleAttackGroups()[0], enemy);
	}
}

///finds an idle group to scout with
void GroupController::ScoutWithIdleGroup()
{
	ai->utility->Log(ALL, MISC, "ScoutWithIdleGroup::IsScoutGroupAvailable()");
	if(MilitaryGroupMgr->IsScoutGroupAvailable())
	{
		ai->utility->Log(ALL, MISC, "ScoutWithIdleGroup::IsScoutGroupAvailable() TRUE");
		MilitaryGroupMgr->GiveScoutOrder(MilitaryGroupMgr->GetIdleScoutGroups()[0]);
	}
}

void GroupController::MoveGroupToPosition(SAIFloat3 pos)
{
	ai->utility->Log(ALL, MISC, "MoveGroupToPosition::IsAttackGroupAvailable()");
	if(MilitaryGroupMgr->IsAttackGroupAvailable())
	{
		ai->utility->Log(ALL, MISC, "MoveGroupToPosition::IsAttackGroupAvailable() TRUE");
		MilitaryGroupMgr->GiveMoveOrder(MilitaryGroupMgr->GetIdleAttackGroups()[0], pos);
	}
}

MilitaryGroupManager* GroupController::GetMilitaryGroupMgr()
{
	return MilitaryGroupMgr;
}

ConstructionGroupManager* GroupController::GetConstructionGroupMgr()
{
	return ConstructionGroupMgr;
}


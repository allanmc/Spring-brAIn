#include "GroupController.h"


brainSpace::GroupController::GroupController(void)
{
	ConstructionGroupMgr = new ConstructionGroupManager();
	MilitaryGroupMgr = new MilitaryGroupManager();
}

brainSpace::GroupController::~GroupController(void)
{
}

void brainSpace::GroupController::AddUnit(Unit *unit)
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

void brainSpace::GroupController::RemoveUnit(Unit *unit)
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
#include "ConstructionGroupManager.h"



ConstructionGroupManager::ConstructionGroupManager( AICallback* clb )
{
	Callback = clb;
}

ConstructionGroupManager::~ConstructionGroupManager()
{
}

void ConstructionGroupManager::AddUnit( Unit* unit )
{
	if ( UnitGroups.size() == 0 )
	{
		UnitGroups.push_back( new ConstructionUnitGroup( Callback ) );
		UnitGroups[0]->AddUnit( unit );
		return;
	}

	BrainGroup* smallestSet = (*UnitGroups.begin());
	for ( int i = 0 ; i < UnitGroups.size() ; i++ )
	{
		if ( UnitGroups[i]->GetSize() < smallestSet->GetSize() )
		{
			smallestSet = UnitGroups[i];
		}
	}

	smallestSet->AddUnit( unit );
}

void ConstructionGroupManager::RemoveUnit( Unit* unit )
{

}

int ConstructionGroupManager::DelegateBuildOrder(SBuildUnitCommand order)
{
	for ( int i = 0 ; i < UnitGroups.size() ; i++ )
	{
		if ( UnitGroups[i]->IsIdle() && UnitGroups[i]->GetSize() > 0 )
		{
			UnitGroups[i]->AssignBuildOrder( order );
			return 1;
		}
	}
	//No groups were idle.

}
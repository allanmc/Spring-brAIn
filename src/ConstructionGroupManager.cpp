#include "ConstructionGroupManager.h"

using namespace std;
using namespace springai;
using namespace brainSpace;

ConstructionGroupManager::ConstructionGroupManager( AIClasses* aiClasses )
{
	ai = aiClasses;
}

ConstructionGroupManager::~ConstructionGroupManager()
{
}

void ConstructionGroupManager::AddUnit( Unit* unit )
{
	if ( UnitGroups.size() == 0 )
	{
		UnitGroups.push_back( new ConstructionUnitGroup( ai, 0 ) );
		UnitGroups[0]->AddUnit( unit );
		return;
	}

	BrainGroup* smallestSet = (*UnitGroups.begin());
	for ( int i = 0 ; i < (int)UnitGroups.size() ; i++ )
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

///finds an idle group to construct the building specified
///@param order contains all information needed except the contructor
///@return 1 if an idle group was found, or 0 if the order was queued
int ConstructionGroupManager::DelegateBuildOrder(SBuildUnitCommand order)
{
	ai->utility->ChatMsg( "Unitgroups size: %d", UnitGroups.size() );
	for ( int i = 0 ; i < (int)UnitGroups.size() ; i++ )
	{
		if ( UnitGroups[i]->IsIdle() && UnitGroups[i]->GetSize() > 0 )
		{
			ai->utility->ChatMsg( "Assigning order to unit group %d", i );
			UnitGroups[i]->AssignBuildOrder( order );
			return 1;
		}
	}
	//No groups were idle so queue the build order.
	//TODO: do this smarter -- check which group becomes idle soonest (aggregated remaining build time something something)
	UnitGroups[0]->QueueBuildOrder( order );
	
	return 0;
}

///tells the group that a given unit have gone idle
void ConstructionGroupManager::UnitIdle( Unit* unit )
{
	UnitGroups[0]->SetAvailable();
}

///
bool ConstructionGroupManager::IsIdle()
{
	return UnitGroups[0]->IsIdle();
}
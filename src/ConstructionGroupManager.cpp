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
	//ai->utility->ChatMsg("Deconstructing ConstructionGroupManager");
	for (int i = 0; i < (int)UnitGroups.size(); i++)
	{
		delete UnitGroups[i];
		UnitGroups[i] = NULL;
	}
	UnitGroups.clear();
}

void ConstructionGroupManager::AddUnit( Unit* unit )
{
	if ( UnitGroups.size() == 0 )
	{
		ai->utility->Log(ALL, MISC,"I am now adding a unit to a new group");
		UnitGroups.push_back( new ConstructionUnitGroup( ai, 0 ) );
		UnitGroups[0]->AddUnit( unit );
		ai->utility->Log(ALL, MISC,"Done adding");
		return;
	}

	BrainGroup* smallestSet = (*UnitGroups.begin());
	ai->utility->Log(ALL, MISC,"I am now adding a unit to an old group");
	for ( int i = 0 ; i < (int)UnitGroups.size() ; i++ )
	{
		if ( UnitGroups[i]->GetSize() < smallestSet->GetSize() )
		{
			smallestSet = UnitGroups[i];
		}
	}
	ai->utility->Log(ALL, MISC,"Done adding");
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
	ai->utility->Log(ALL, MISC,  "Unitgroups size: %d", UnitGroups.size() );
	for ( int i = 0 ; i < (int)UnitGroups.size() ; i++ )
	{
		if ( UnitGroups[i]->IsIdle() && UnitGroups[i]->GetSize() > 0 )
		{
			ai->utility->Log(ALL, MISC,  "Assigning order to unit group %d", i );
			UnitGroups[i]->AssignBuildOrder( order );
			return 1;
		}
	}
	//No groups were idle so queue the build order.
	//TODO: do this smarter -- check which group becomes idle soonest (aggregated remaining build time something something)
	
	if (UnitGroups.size()==0)
	{
		ai->utility->Log(ALL, MISC,  "Could not queue order, no group " );
		return 0;
	}
	ai->utility->Log(ALL, MISC,  "Quing order, no idle group " );
	UnitGroups[0]->QueueBuildOrder( order );
	
	return 0;
}

///tells the group that a given unit have gone idle
void ConstructionGroupManager::UnitIdle( Unit* unit )
{
	ai->utility->Log(ALL, MISC, "ConstructionGroupManager::UnitIdle()");
	if (UnitGroups.size()==0)
	{
		ai->utility->Log(ALL, MISC,  "Could not set unit idle, no group " );
		return;
	}
	UnitGroups[0]->SetAvailable();
}

///
bool ConstructionGroupManager::IsIdle()
{
	if (UnitGroups.size()==0)
	{
		ai->utility->Log(ALL, MISC,  "Could not chekc IsIdle(), no group " );
		return false;
	}
	return UnitGroups[0]->IsIdle();
}

int brainSpace::ConstructionGroupManager::GetAmountOfBuildOrdersForGroup0()
{
	return UnitGroups[0]->GetBuildQueueSize();
}
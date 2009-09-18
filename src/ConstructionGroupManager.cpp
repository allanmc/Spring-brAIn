#include "ConstructionGroupManager.h"



brainSpace::ConstructionGroupManager::ConstructionGroupManager()
{
}

brainSpace::ConstructionGroupManager::~ConstructionGroupManager()
{
}

void brainSpace::ConstructionGroupManager::AddUnit( Unit* unit )
{
	if ( UnitGroups.size() == 0 )
	{
		UnitGroups.push_back( new BrainGroup() );
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

void brainSpace::ConstructionGroupManager::RemoveUnit( Unit* unit )
{

}
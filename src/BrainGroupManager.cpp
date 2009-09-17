#include "BrainGroupManager.h"

BrainGroupManager::BrainGroupManager()
{
}

BrainGroupManager::~BrainGroupManager()
{
}

void BrainGroupManager::AddUnit( Unit* unit )
{
	vector<BrainGroup*>::iterator iter;
	
	BrainGroup* smallestGroup = (*Groups.begin());

	if ( Groups.size() == 0 )
	{
		return;
	}

	for ( int i = 0 ; i < Groups.size() ; i++ )
	{
		if ( Groups[i]->GetSize() < smallestGroup->GetSize() )
		{
			smallestGroup = Groups[i];
		}
	}

	smallestGroup->AddUnit( unit );
}

void BrainGroupManager::RemoveUnit( Unit* unit )
{
}
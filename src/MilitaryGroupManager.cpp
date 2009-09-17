#include "MilitaryGroupManager.h"


brainSpace::MilitaryGroupManager::MilitaryGroupManager()
{
}


brainSpace::MilitaryGroupManager::~MilitaryGroupManager()
{
}

void brainSpace::MilitaryGroupManager::AddUnit( Unit* unit )
{
	set<set<Unit*>*>::iterator iter;
	
	set<Unit*>* smallestSet = (*UnitGroups.begin());

	if ( UnitGroups.size() == 0 )
	{
		return;
	}
	for ( iter = UnitGroups.begin() ; iter != UnitGroups.end() ; iter ++ )
	{
		set<Unit*>* currentSet = (*iter);
		if ( currentSet->size() < smallestSet->size() )
		{
			smallestSet = currentSet;
		}
	}

	smallestSet->insert( unit );
}

void brainSpace::MilitaryGroupManager::RemoveUnit( Unit* unit )
{
}
#include "ConstructionGroupManager.h"



brainSpace::ConstructionGroupManager::ConstructionGroupManager()
{
}

brainSpace::ConstructionGroupManager::~ConstructionGroupManager()
{
}

void brainSpace::ConstructionGroupManager::AddUnit( Unit* unit )
{
	set<BrainGroup*>::iterator iter;
	BrainGroup* smallestSet = (*UnitGroups.begin());
	
	Utility::GetInstance()->ChatMsg( "AddUnit - Const Mgr" );
	if ( UnitGroups.size() == 0 )
	{
		Utility::GetInstance()->ChatMsg( "UnitGroups size 0" );
		return;
	}
	for ( iter = UnitGroups.begin() ; iter != UnitGroups.end() ; iter ++ )
	{
		BrainGroup* currentSet = (*iter);

		if ( currentSet->GetSize() < smallestSet->GetSize() )
		{
			smallestSet = currentSet;
		}
	}
	smallestSet->AddUnit( unit );
}

void brainSpace::ConstructionGroupManager::RemoveUnit( Unit* unit )
{

}
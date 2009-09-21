#include "MilitaryGroupManager.h"


brainSpace::MilitaryGroupManager::MilitaryGroupManager( AICallback* callback )
{
	Callback = callback;
	MilitaryUnitGroup* group1 = new MilitaryUnitGroup(Callback);
	UnitGroups.push_back(group1);
}


brainSpace::MilitaryGroupManager::~MilitaryGroupManager()
{
}

void brainSpace::MilitaryGroupManager::AddUnit( Unit* unit )
{
	MilitaryUnitGroup* tmpGroup = NULL;
	for(int i = 0; i < UnitGroups.size(); i++)
	{
		if(UnitGroups[i]->GetSize() < 10)
		{
			tmpGroup = UnitGroups[i];
			break;
		}
	}
	if(tmpGroup == NULL)
	{
		tmpGroup = new MilitaryUnitGroup(Callback);
		UnitGroups.push_back(tmpGroup);
	}
	tmpGroup->AddUnit(unit);
	if(tmpGroup->GetSize() > 9)
	{
		tmpGroup->SetStatus(MilitaryUnitGroup::Idle);
	}
}

void brainSpace::MilitaryGroupManager::RemoveUnit( Unit* unit )
{
	for(int i = 0; i < UnitGroups.size(); i++)
	{
		UnitGroups[i]->RemoveUnit(unit);
	}
}

vector<MilitaryUnitGroup*> MilitaryGroupManager::GetIdleGroups()
{
	vector<MilitaryUnitGroup*> result;
	for(int i = 0; i < UnitGroups.size(); i++)
	{
		if(UnitGroups[i]->GetStatus() == MilitaryUnitGroup::Idle)
		{
			result.push_back(UnitGroups[i]);
		}
	}
}

void MilitaryGroupManager::GiveAttackOrder(brainSpace::MilitaryUnitGroup* group, int enemy)
{
	group->SetStatus(MilitaryUnitGroup::Attacking);
	
}

void MilitaryGroupManager::GiveScoutOrder(brainSpace::MilitaryUnitGroup* group, SAIFloat3 pos)
{
	group->SetStatus(MilitaryUnitGroup::Scouting);
}
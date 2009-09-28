#include "MilitaryGroupManager.h"


brainSpace::MilitaryGroupManager::MilitaryGroupManager( AIClasses* aiClasses )
{
	ai = aiClasses;
	MilitaryUnitGroup* group1 = new MilitaryUnitGroup( ai );
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
		tmpGroup = new MilitaryUnitGroup( ai );
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
		if(UnitGroups[i]->GetSize() == 0)
		{
			MilitaryUnitGroup* tmp = UnitGroups[i];
			UnitGroups.erase(UnitGroups.begin() + i);
			delete tmp;
		}
	}
}

vector<MilitaryUnitGroup*> MilitaryGroupManager::GetIdleGroups()
{
	//Utility* u = new Utility(Callback);
//	u->ChatMsg("Creating vector with idle groups");
	vector<MilitaryUnitGroup*> result;
	
	for(int i = 0; i < UnitGroups.size(); i++)
	{
		if(UnitGroups[i]->GetStatus() == MilitaryUnitGroup::Idle)
		{
			//u->ChatMsg("Adding an idle group to vector");
			result.push_back(UnitGroups[i]);
		}
	}
	return result;
}

void MilitaryGroupManager::GiveAttackOrder(brainSpace::MilitaryUnitGroup* group, int enemy)
{
	group->SetStatus(MilitaryUnitGroup::Attacking);
	group->Attack(enemy);
	
}

void MilitaryGroupManager::GiveScoutOrder(brainSpace::MilitaryUnitGroup* group)
{
	group->SetStatus(MilitaryUnitGroup::Scouting);
	int h = ai->callback->GetMap()->GetHeight();
	int w = ai->callback->GetMap()->GetWidth();
	ai->utility->ChatMsg("Height:%d", h);
	ai->utility->ChatMsg("Width:%d", w);
	SAIFloat3 pos;
	pos.x = (rand() % w)*8;
	pos.z = (rand() % h)*8;
	pos.y = 0;
	ai->utility->ChatMsg("newHeight:%f", pos.x);
	ai->utility->ChatMsg("newWidth:%f", pos.z);
	ai->utility->ChatMsg("random generation scout position done");
	ai->utility->DrawCircle(group->GetPos(), 100);
	group->Scout(pos);

}

void MilitaryGroupManager::UnitIdle(Unit* unit)
{
	for(int i = 0; i < UnitGroups.size(); i++)
	{
		UnitGroups[i]->UnitIdle(unit);
	}

}

vector<MilitaryUnitGroup*> MilitaryGroupManager::GetNonAttackingGroups()
{
	vector<MilitaryUnitGroup*> result;
	
	for(int i = 0; i < UnitGroups.size(); i++)
	{
		if(UnitGroups[i]->GetStatus() == MilitaryUnitGroup::Idle || UnitGroups[i]->GetStatus() == MilitaryUnitGroup::Scouting)
		{			
			result.push_back(UnitGroups[i]);
		}
	}
	return result;
}

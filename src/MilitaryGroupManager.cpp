#include "MilitaryGroupManager.h"

using namespace std;
using namespace springai;
using namespace brainSpace;

#include "BrainMap.h"
#include "ScoutMap.h"
#include "ThreatMap.h"
#include "Knowledge.h"

brainSpace::MilitaryGroupManager::MilitaryGroupManager( AIClasses* aiClasses )
{
	ai = aiClasses;
	MilitaryUnitGroup* group1 = new MilitaryUnitGroup( ai, 0 );
	UnitGroups.push_back(group1);
	MilitaryUnitGroup* group2 = new MilitaryUnitGroup( ai, 0 );
	ScoutGroups.push_back(group2);
}


brainSpace::MilitaryGroupManager::~MilitaryGroupManager()
{
	for (int i = 0; i < (int)UnitGroups.size(); i++)
	{
		delete UnitGroups[i];
		UnitGroups[i] = NULL;
	}
	UnitGroups.clear();
	for (int i = 0; i < (int)ScoutGroups.size(); i++)
	{
		delete ScoutGroups[i];
		ScoutGroups[i] = NULL;
	}
	ScoutGroups.clear();
}

void brainSpace::MilitaryGroupManager::AddUnit( Unit* unit )
{
	MilitaryUnitGroup* tmpGroup = NULL;
	UnitDef *def = unit->GetDef();
	if(strcmp(def->GetName(),"armfav") != 0 && strcmp(def->GetName(), "armflea") != 0)
	{
		for(int i = 0; i < (int)UnitGroups.size(); i++)
		{
			if(UnitGroups[i]->GetSize() < 10 && UnitGroups[i]->GetStatus() == MilitaryUnitGroup::MILI_UNIT_GRP_REGROUPING )
			{
				tmpGroup = UnitGroups[i];
				break;
			}
		}
		if(tmpGroup == NULL)
		{
			tmpGroup = new MilitaryUnitGroup( ai, UnitGroups.size() );
			UnitGroups.push_back(tmpGroup);
		}
		if(tmpGroup->GetSize() > 8)
		{
			tmpGroup->SetStatus(MilitaryUnitGroup::MILI_UNIT_GRP_IDLE);
			SAIFloat3 pos;
			pos.x = ai->callback->GetMap()->GetWidth()/2;
			pos.z = ai->callback->GetMap()->GetHeight()/2;
			pos.y = 50;
			tmpGroup->Scout(pos);
		}
	}
	else
	{
		for(int i = 0; i < (int)ScoutGroups.size(); i++)
		{
			if(ScoutGroups[i]->GetSize() < 3 && ScoutGroups[i]->GetStatus() == MilitaryUnitGroup::MILI_UNIT_GRP_REGROUPING )
			{
				tmpGroup = ScoutGroups[i];
				break;
			}
		}
		if(tmpGroup == NULL)
		{
			tmpGroup = new MilitaryUnitGroup( ai, ScoutGroups.size() );
			ScoutGroups.push_back(tmpGroup);
		}
		if(tmpGroup->GetSize() > 1)
		{
			tmpGroup->SetStatus(MilitaryUnitGroup::MILI_UNIT_GRP_IDLE);
		}
	}
	tmpGroup->AddUnit(unit);
	ai->utility->Log( ALL, SCOUTING, "Unit %d added to unitgroup %d", unit->GetUnitId(), tmpGroup->GetGroupID() );
	delete def;
}

void brainSpace::MilitaryGroupManager::RemoveUnit( Unit* unit )
{
	UnitDef *def = unit->GetDef();
	if(strcmp(def->GetName(),"armfav") != 0 && strcmp(def->GetName(), "armflea") != 0)
	{
		for(int i = 0; i < (int)UnitGroups.size(); i++)
		{
			UnitGroups[i]->RemoveUnit(unit);
			if(UnitGroups[i]->GetSize() == 0)
			{
				MilitaryUnitGroup* tmp = UnitGroups[i];
				UnitGroups.erase(UnitGroups.begin() + i);
				delete tmp;
				tmp = NULL;
			}
		}
	}
	else
	{
		for(int i = 0; i < (int)ScoutGroups.size(); i++)
		{
			ScoutGroups[i]->RemoveUnit(unit);
			if(ScoutGroups[i]->GetSize() == 0)
			{
				MilitaryUnitGroup* tmp = ScoutGroups[i];
				ScoutGroups.erase(ScoutGroups.begin() + i);
				delete tmp;
				tmp = NULL;
			}
		}
	}
	delete def;
}

vector<MilitaryUnitGroup*> brainSpace::MilitaryGroupManager::GetAllAttackGroups()
{
	return UnitGroups;
}
vector<MilitaryUnitGroup*> MilitaryGroupManager::GetIdleAttackGroups()
{
	//Utility* u = new Utility(Callback);
//	u->ChatMsg("Creating vector with idle groups");
	vector<MilitaryUnitGroup*> result;
	
	for(int i = 0; i < (int)UnitGroups.size(); i++)
	{
		if(UnitGroups[i]->GetStatus() == MilitaryUnitGroup::MILI_UNIT_GRP_IDLE)
		{
			//ai->utility->Log(ALL, MISC, "Adding an idle group to vector");
			result.push_back(UnitGroups[i]);
		}
	}
	return result;
}

vector<MilitaryUnitGroup*> MilitaryGroupManager::GetIdleScoutGroups()
{
	vector<MilitaryUnitGroup*> result;
	
	for(int i = 0; i < (int)ScoutGroups.size(); i++)
	{
		if(ScoutGroups[i]->GetStatus() == MilitaryUnitGroup::MILI_UNIT_GRP_IDLE)
		{
			//ai->utility->Log(ALL, MISC, "Adding an idle group to vector");
			result.push_back(ScoutGroups[i]);
		}
	}
	return result;
}

///Tells the group to attack the given enemy.
void MilitaryGroupManager::GiveAttackOrder(brainSpace::MilitaryUnitGroup* group, int enemy)
{
	group->SetStatus(MilitaryUnitGroup::MILI_UNIT_GRP_ATTACKING);
	group->Attack(enemy);
	
}

///Tells the group to scout the cell with the best scouting value
void MilitaryGroupManager::GiveScoutOrder(brainSpace::MilitaryUnitGroup* group)
{
	group->SetStatus(MilitaryUnitGroup::MILI_UNIT_GRP_SCOUTING);
	

	const MapData* scoutMapData = ai->knowledge->mapInfo->scoutMap->GetMapData();
	const MapData* threatMapData = ai->knowledge->mapInfo->threatMap->GetMapData();
	const MapData* resourceMapData = ai->knowledge->mapInfo->resourceMap->GetMapData();

	vector<Point*> points = ai->callback->GetMap()->GetPoints(true);
	SAIFloat3 enemyStartingPosition;
	for( int i = 0 ; i < (int)points.size() ; i++ )
	{
		if ( points[i]->GetPosition().x == ai->callback->GetMap()->GetStartPos().x &&
			 points[i]->GetPosition().z == ai->callback->GetMap()->GetStartPos().z )
		{
			continue;
		}
		enemyStartingPosition = points[i]->GetPosition();
		break;
	}


	int startIndex = floorf(enemyStartingPosition.z/scoutMapData->MapResolution)*scoutMapData->MapWidth + floorf(enemyStartingPosition.x/scoutMapData->MapResolution);

	float highestReward = -9999.28f;
	SAIFloat3 bestScoutingPos;

	ai->utility->Log( ALL, SCOUTING, "------" );
	for ( int i = 0 ; i < scoutMapData->MapHeight*scoutMapData->MapWidth ; i++ )
	{
		float reward = 0.0f;
		SAIFloat3 tilePosition;
		tilePosition.x = (i%scoutMapData->MapWidth+0.5f)*scoutMapData->MapResolution;
		tilePosition.y = 0;
		tilePosition.z = (i/scoutMapData->MapWidth+0.5f)*scoutMapData->MapResolution;

		double distance = ai->utility->EuclideanDistance( tilePosition, group->GetPos() );		
		int threat = threatMapData->MapArray[i];
		int age = ai->frame - scoutMapData->MapArray[i];

		reward -= distance*SCOUT_REWARD_DISTANCE;
		reward -= threat*SCOUT_REWARD_THREAT;
		if ( threat != 0 )
			ai->utility->Log( ALL, SCOUTING, "threat not zero: %d", threat );
		reward += age*SCOUT_REWARD_AGE;
		reward += resourceMapData->MapArray[i]*SCOUT_REWARD_METAL_SPOTS;
		if ( i == startIndex )
			reward += SCOUT_REWARD_START_POS;


		ai->utility->Log( ALL, SCOUTING, "Reward %.2f at (%d, %d): distance %.2f", reward, i%scoutMapData->MapWidth, i/scoutMapData->MapWidth, distance );
		ai->utility->Log( ALL, SCOUTING, "Age: %.2f - Distance: %.2f - Threat: %.2f + MetalSpot: %.2f + StartPos: %.2f", age*SCOUT_REWARD_AGE, distance*SCOUT_REWARD_DISTANCE , threat*SCOUT_REWARD_THREAT, resourceMapData->MapArray[i]*SCOUT_REWARD_METAL_SPOTS, SCOUT_REWARD_START_POS );

		if ( reward > highestReward )
		{
			highestReward = reward;
			bestScoutingPos = tilePosition;
			ai->utility->Log( ALL, SCOUTING, "NEW BEST!!" );		
		}
	}
	ai->utility->Log( ALL, SCOUTING, "------" );
	//ai->utility->Log(ALL, MISC, "newHeight:%f", pos.x);
	//ai->utility->Log(ALL, MISC, "newWidth:%f", pos.z);
	//ai->utility->Log(ALL, MISC, "random generation scout position done");
	group->Scout(bestScoutingPos);
}

///informs the groups that a unit has gone idle
void MilitaryGroupManager::UnitIdle(Unit* unit)
{
	if(unit->GetCurrentCommands().size() > 0)
		return;
	UnitDef *def = unit->GetDef();
	if(strcmp(def->GetName(),"armfav") != 0 && strcmp(def->GetName(), "armflea") != 0)
	{
		for(int i = 0; i < (int)UnitGroups.size(); i++)
		{
			UnitGroups[i]->UnitIdle(unit);
		}
	}
	else
	{
		for(int i = 0; i < (int)ScoutGroups.size(); i++)
		{
			ScoutGroups[i]->UnitIdle(unit);
		}
	}
	delete def;
}

bool MilitaryGroupManager::IsAllAttackGroupsIdle()
{
	for(int i = 0; i < UnitGroups.size(); i++)
	{
		if(!UnitGroups[i]->IsIdle())
			return false;
	}
	return true;
}

bool MilitaryGroupManager::IsAllScoutGroupsIdle()
{
	for(int i = 0; i < ScoutGroups.size(); i++)
	{
		if(!ScoutGroups[i]->IsIdle())
			return false;
	}
	return true;
}

bool MilitaryGroupManager::IsAttackGroupAvailable()
{
	if(GetIdleAttackGroups().size() > 0)
		return true;
	else
		return false;
}

bool MilitaryGroupManager::IsScoutGroupAvailable()
{
	if(GetIdleScoutGroups().size() > 0)
		return true;
	else
		return false;
}

void MilitaryGroupManager::GuardUnit(springai::Unit *unit)
{
	vector<MilitaryUnitGroup*> groups = GetIdleAttackGroups();
	groups[0]->Guard(unit->GetUnitId());
}

void MilitaryGroupManager::GiveMoveOrder(MilitaryUnitGroup* group, SAIFloat3 pos)
{
	group->Scout(pos);
}

int MilitaryGroupManager::GetNumAttackingGroups()
{
	int result = 0;
	
	for(int i = 0; i < (int)UnitGroups.size(); i++)
	{
		if(UnitGroups[i]->GetStatus() == MilitaryUnitGroup::MILI_UNIT_GRP_ATTACKING)
		{
			//ai->utility->Log(ALL, MISC, "Adding an idle group to vector");
			i++;
		}
	}
	return result;
}

int MilitaryGroupManager::GetNumScoutingGroups()
{
	int result = 0;
	
	for(int i = 0; i < (int)ScoutGroups.size(); i++)
	{
		if(ScoutGroups[i]->GetStatus() == MilitaryUnitGroup::MILI_UNIT_GRP_SCOUTING)
		{
			//ai->utility->Log(ALL, MISC, "Adding an idle group to vector");
			i++;
		}
	}
	return result;
}
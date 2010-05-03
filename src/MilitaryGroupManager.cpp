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

bool brainSpace::MilitaryGroupManager::AddUnit( Unit* unit )
{
	UnitDef *def = unit->GetDef();
	if ( def->GetSpeed() <= 0 )
	{
		delete def;
		return false;
	}
	bool inserted = false;
	if(strcmp(def->GetName(),"armpeep") != 0 && strcmp(def->GetName(), "armflea") != 0)
	{
		for(int i = 0; i < (int)UnitGroups.size(); i++)
		{
			double distance = ai->utility->EuclideanDistance( unit->GetPos(), UnitGroups[i]->GetPos() );
			if ( distance < GROUP_PROXIMITY )
			{
				if ( UnitGroups[i]->AddUnit( unit ) )
				{
					inserted = true;
				}
			}
		}
		if( !inserted )
		{
			//ai->utility->ChatMsg("Making new group");
			MilitaryUnitGroup* m = new MilitaryUnitGroup( ai, UnitGroups.size(), def->IsAbleToFly() );
			UnitGroups.push_back(m);
			if (  UnitGroups.back()->AddUnit( unit ))
			{
				inserted = true;
			}
		}
	}
	else
	{
		for(int i = 0; i < (int)ScoutGroups.size(); i++)
		{
			if ( ai->utility->EuclideanDistance( unit->GetPos(), UnitGroups[i]->GetPos() ) < GROUP_PROXIMITY )
			{
				if ( ScoutGroups[i]->AddUnit( unit ) )
				{
					inserted = true;
				}
			}
		}
		if( !inserted )
		{
			MilitaryUnitGroup* m = new MilitaryUnitGroup( ai, UnitGroups.size(), def->IsAbleToFly() );
			ScoutGroups.push_back(m);
		}
	}
	delete def;
	return inserted;
}

void brainSpace::MilitaryGroupManager::RemoveUnit( Unit* unit )
{
	UnitDef *def = unit->GetDef();
	if(strcmp(def->GetName(),"armpeep") != 0 && strcmp(def->GetName(), "armflea") != 0)
	{
		for(int i = 0; i < (int)UnitGroups.size(); i++)
		{
			UnitGroups[i]->RemoveUnit(unit);
			if(UnitGroups[i]->GetSize() == 0)
			{
				//ai->utility->ChatMsg("MilitaryGroup is empty because all units in it were killed");
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
		else ai->utility->ChatMsg("Un-idle group found");
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

///Tells the group to attack the given enemy.
void MilitaryGroupManager::GiveAttackOrder(brainSpace::MilitaryUnitGroup* group, vector<int> enemies)
{
	group->SetStatus(MilitaryUnitGroup::MILI_UNIT_GRP_ATTACKING);
	group->Attack(enemies);
	
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
bool MilitaryGroupManager::UnitIdle(Unit* unit)
{
	bool allIdle = false;
	if(unit->GetCurrentCommands().size() > 0)
		return allIdle;
	UnitDef *def = unit->GetDef();
	if(strcmp(def->GetName(),"armpeep") != 0 && strcmp(def->GetName(), "armflea") != 0)
	{
		for(int i = 0; i < (int)UnitGroups.size(); i++)
		{
			allIdle = UnitGroups[i]->UnitIdle(unit);
		}
	}
	else
	{
		for(int i = 0; i < (int)ScoutGroups.size(); i++)
		{
			allIdle = ScoutGroups[i]->UnitIdle(unit);
		}
	}
	delete def;
	return allIdle;
}

bool MilitaryGroupManager::IsAllAttackGroupsIdle()
{
	for(unsigned int i = 0; i < UnitGroups.size(); i++)
	{
		if(!UnitGroups[i]->IsIdle())
			return false;
	}
	return true;
}

bool MilitaryGroupManager::IsAllScoutGroupsIdle()
{
	for(unsigned int i = 0; i < ScoutGroups.size(); i++)
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


MilitaryUnitGroup* MilitaryGroupManager::GetNearestGroup( MilitaryUnitGroup* group )
{
	double closest = 100000.0f;
	MilitaryUnitGroup* ret = NULL;
	for ( unsigned int i = 0 ; i < UnitGroups.size() ; i++ )
	{
		if ( UnitGroups[i]->GetGroupID() != group->GetGroupID() )
		{
			if ( double dist = ai->utility->EuclideanDistance( UnitGroups[i]->GetPos(), group->GetPos() ) < closest )
			{
				closest = dist;
				ret = UnitGroups[i];
			}
		}
	}
	return ret;
}
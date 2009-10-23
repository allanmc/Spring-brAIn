#include "MilitaryGroupManager.h"

using namespace std;
using namespace springai;
using namespace brainSpace;

#include "BrainMap.h"
#include "ScoutMap.h"
#include "ThreatMap.h"


brainSpace::MilitaryGroupManager::MilitaryGroupManager( AIClasses* aiClasses )
{
	ai = aiClasses;
	MilitaryUnitGroup* group1 = new MilitaryUnitGroup( ai, 0 );
	UnitGroups.push_back(group1);
}


brainSpace::MilitaryGroupManager::~MilitaryGroupManager()
{
}

void brainSpace::MilitaryGroupManager::AddUnit( Unit* unit )
{
	MilitaryUnitGroup* tmpGroup = NULL;
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
	tmpGroup->AddUnit(unit);
	ai->utility->Log( ALL, SCOUTING, "Unit %d added to unitgroup %d", unit->GetUnitId(), tmpGroup->GetGroupID() );
	if(tmpGroup->GetSize() > 9)
	{
		tmpGroup->SetStatus(MilitaryUnitGroup::MILI_UNIT_GRP_IDLE);
	}
}

void brainSpace::MilitaryGroupManager::RemoveUnit( Unit* unit )
{
	for(int i = 0; i < (int)UnitGroups.size(); i++)
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
	
	for(int i = 0; i < (int)UnitGroups.size(); i++)
	{
		if(UnitGroups[i]->GetStatus() == MilitaryUnitGroup::MILI_UNIT_GRP_IDLE)
		{
			//ai->utility->ChatMsg("Adding an idle group to vector");
			result.push_back(UnitGroups[i]);
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
			 points[i]->GetPosition().y == ai->callback->GetMap()->GetStartPos().y &&
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
	//ai->utility->ChatMsg("newHeight:%f", pos.x);
	//ai->utility->ChatMsg("newWidth:%f", pos.z);
	//ai->utility->ChatMsg("random generation scout position done");
	group->Scout(bestScoutingPos);
}

///informs the groups that a unit has gone idle
void MilitaryGroupManager::UnitIdle(Unit* unit)
{
	for(int i = 0; i < (int)UnitGroups.size(); i++)
	{
		UnitGroups[i]->UnitIdle(unit);
	}
}

///@return the groups that are not currently attacking enemies.
vector<MilitaryUnitGroup*> MilitaryGroupManager::GetNonAttackingGroups()
{
	vector<MilitaryUnitGroup*> result;
	
	for(int i = 0; i < (int)UnitGroups.size(); i++)
	{
		if(UnitGroups[i]->GetStatus() == MilitaryUnitGroup::MILI_UNIT_GRP_IDLE || UnitGroups[i]->GetStatus() == MilitaryUnitGroup::MILI_UNIT_GRP_SCOUTING)
		{			
			result.push_back(UnitGroups[i]);
		}
	}
	return result;
}

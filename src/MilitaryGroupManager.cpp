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

///Tells the group to attack the given enemy.
void MilitaryGroupManager::GiveAttackOrder(brainSpace::MilitaryUnitGroup* group, int enemy)
{
	group->SetStatus(MilitaryUnitGroup::Attacking);
	group->Attack(enemy);
	
}

///Tells the group to scout the cell with the best scouting value
void MilitaryGroupManager::GiveScoutOrder(brainSpace::MilitaryUnitGroup* group)
{
	group->SetStatus(MilitaryUnitGroup::Scouting);
	int h = ai->callback->GetMap()->GetHeight();
	int w = ai->callback->GetMap()->GetWidth();
	//ai->utility->ChatMsg("Height:%d", h);
	//ai->utility->ChatMsg("Width:%d", w);
	



	const MapData* scoutMapData = ai->knowledge->mapInfo->scoutMap->GetMapData();
	const MapData* threatMapData = ai->knowledge->mapInfo->threatMap->GetMapData();
	const MapData* resourceMapData = ai->knowledge->mapInfo->resourceMap->GetMapData();

	vector<Point*> points = ai->callback->GetMap()->GetPoints(true);
	SAIFloat3 enemyStartingPosition;
	ai->utility->Log( DEBUG, KNOWLEDGE, "Points %d", points.size() );
	ai->utility->ChatMsg( "Points %d", points.size() );
	for( int i = 0 ; i < points.size() ; i++ )
	{
		if ( points[i]->GetPosition().x == ai->callback->GetMap()->GetStartPos().x &&
			 points[i]->GetPosition().y == ai->callback->GetMap()->GetStartPos().y &&
			 points[i]->GetPosition().z == ai->callback->GetMap()->GetStartPos().z )
		{
			continue;
		}
		enemyStartingPosition = points[i]->GetPosition();
		ai->utility->ChatMsg( "Found enemy pos:" );
		break;
	}


	int startIndex = floorf(enemyStartingPosition.z/scoutMapData->MapResolution)*scoutMapData->MapWidth + floorf(enemyStartingPosition.x/scoutMapData->MapResolution);

	float highestReward = -9999.28;
	SAIFloat3 bestScoutingPos;

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
		reward += age*SCOUT_REWARD_AGE;
		reward += resourceMapData->MapArray[i]*SCOUT_REWARD_METAL_SPOTS;
		if ( i == startIndex )
			reward += SCOUT_REWARD_START_POS;

		if ( reward > highestReward )
		{
			highestReward = reward;
			bestScoutingPos = tilePosition;
			ai->utility->ChatMsg( "New best reward %f at tile (%d, %d)", reward, i%scoutMapData->MapWidth, i/scoutMapData->MapWidth );
			ai->utility->Log( DEBUG, KNOWLEDGE, "New best reward %f at tile (%d, %d)", reward, i%scoutMapData->MapWidth, i/scoutMapData->MapWidth );
		}
	}
	//ai->utility->ChatMsg("newHeight:%f", pos.x);
	//ai->utility->ChatMsg("newWidth:%f", pos.z);
	//ai->utility->ChatMsg("random generation scout position done");
	group->Scout(bestScoutingPos);
}

///informs the groups that a unit have gone idle
void MilitaryGroupManager::UnitIdle(Unit* unit)
{
	for(int i = 0; i < UnitGroups.size(); i++)
	{
		UnitGroups[i]->UnitIdle(unit);
	}

}

///@return the groups that are not currently attacking enemies.
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

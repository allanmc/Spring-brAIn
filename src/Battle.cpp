#include "Battle.h"
#include "Knowledge.h"

using namespace std;
using namespace springai;
using namespace brainSpace;

Battle::Battle( AIClasses* aiClasses, SAIFloat3 position )
{
	Center = position;
	Radius = INITIAL_BATTLE_RADIUS;
	ai = aiClasses;
	StartFrame = ai->frame;
	LastFrameOfActivity = ai->frame;
	RadiusCircleID = -666;
	//check if any building is inside our radius, if it is, make type an attack (his or ours).
	bool myattack = ai->knowledge->enemyInfo->baseInfo->IsBuildingInRange(Center, Radius);
	bool hisattack = ai->knowledge->selfInfo->baseInfo->IsBuildingInRange(Center, Radius);
	if(myattack && hisattack)
	{
		ai->utility->Log( LOG_DEBUG, KNOWLEDGE, "bot hisattack and myattack are true.. FUCK!");
	}
	if(hisattack)
	{
		ai->utility->Log( LOG_DEBUG, KNOWLEDGE, "new battle: hisattack");
		BattleLabel = HISATTACK;
	}
	else if(myattack)
	{
		ai->utility->Log( LOG_DEBUG, KNOWLEDGE, "new battle: myattack");
		BattleLabel = MYATTACK;
	}
	else
	{
		ai->utility->Log( LOG_DEBUG, KNOWLEDGE, "new battle: random");
		BattleLabel = RANDOM;
	}


}


Battle::~Battle()
{
}


void Battle::UnitDied( int unitID, bool enemy )
{
	LastFrameOfActivity = ai->frame;
	//ai->utility->Log( LOG_DEBUG, KNOWLEDGE, "UNIT DIED: %d unitdefID: %d", u->GetUnitId(), u->GetDef()->GetUnitDefId() );
	Unit *u = Unit::GetInstance(ai->callback, unitID);
	UnitDef* def = u->GetDef();

	map<int, int>::iterator iter;

	if ( enemy )
	{
		//ai->utility->Log( LOG_DEBUG, KNOWLEDGE, "dead unit is enemy unit" );
		if(def == NULL)
			ai->knowledge->enemyInfo->armyInfo->GetUnitDef(unitID);
		if(def != NULL)
			DeadEnemyUnits[def->GetUnitDefId()] += 1;
		else
			ai->utility->Log( LOG_DEBUG, KNOWLEDGE, "dead unit is enemy unit, but we dont know unitdef!?" );
		ActiveEnemyUnits.erase( unitID );
	}
	else
	{
		//ai->utility->Log( LOG_DEBUG, KNOWLEDGE, "dead unit is friendly unit" );
		if(def != NULL)
		{
			DeadFriendlyUnits[def->GetUnitDefId()] += 1;
		}
		else
			ai->utility->Log( LOG_DEBUG, KNOWLEDGE, "dead unit is friendly unit, but we dont know unitdef!?" );
		ActiveFriendlyUnits.erase( unitID );
	}
}

void Battle::UnitEnteredBattle( int unitID, bool enemy )
{
	ai->utility->Log(ALL, KNOWLEDGE, "UnitEnteredBattle start");
	Unit *u = Unit::GetInstance(ai->callback, unitID);
	if(u == NULL)
		ai->utility->Log(ALL, KNOWLEDGE, "UnitEnteredBattle: WTF, unit is NULL? id: %d, enemy: %d", unitID, enemy);
	UnitInformationContainer container;
	container.def = u->GetDef();
	container.pos = u->GetPos();
	
	ai->utility->Log(ALL, KNOWLEDGE, "UnitEnteredBattle 4");

	//ai->utility->Log( LOG_DEBUG, KNOWLEDGE, "UnitID %d entered battle: defID %d", unitID, container.def->GetUnitDefId() );
	
	if ( container.def == NULL )
	{
		ai->utility->Log(ALL, KNOWLEDGE, "UnitEnteredBattle Unitdef = NULL");
		//ai->utility->Log( LOG_DEBUG, KNOWLEDGE, "defID was -1" );
		UnitDef* def = ai->knowledge->enemyInfo->armyInfo->GetUnitDef( unitID );
		if ( def != NULL )
		{
			ai->utility->Log(ALL, KNOWLEDGE, "UnitEnteredBattle Unitdef = %s", def->GetName());
			container.def = def;
			//ai->utility->Log( LOG_DEBUG, KNOWLEDGE, "Def updated!" );
		}
		//else
			//ai->utility->Log( LOG_DEBUG, KNOWLEDGE, "Def not updated" );
	}
	ai->utility->Log(ALL, KNOWLEDGE, "UnitEnteredBattle 5");
	LastFrameOfActivity = ai->frame;


	map<int, UnitInformationContainer>::iterator iter;
	if ( float distance = (ai->utility->EuclideanDistance( u->GetPos(), Center )) > Radius )
	{
		Radius = distance;
		ai->utility->Log(ALL, KNOWLEDGE, "New radius: %f", Radius );
	}
	if ( !enemy )
	{
		ai->utility->Log(ALL, KNOWLEDGE, "UnitEnteredBattle not enemy");

		ActiveFriendlyUnits[unitID] = container;

		SAIFloat3 pos[ActiveFriendlyUnits.size()];

		int i = 0;
		for ( iter = ActiveFriendlyUnits.begin() ; iter != ActiveFriendlyUnits.end() ; iter++, i++ )
		{
			pos[i] = (*iter).second.pos;
		}
		CalculateCenter( pos, ActiveFriendlyUnits.size() );
	}
	else
	{
		ai->utility->Log(ALL, KNOWLEDGE, "UnitEnteredBattle enemy");
		ActiveEnemyUnits[unitID] = container;

		SAIFloat3 pos[ActiveEnemyUnits.size()];

		int i = 0;
		for ( iter = ActiveEnemyUnits.begin() ; iter != ActiveEnemyUnits.end() ; iter++, i++ )
		{
			pos[i] = (*iter).second.pos;
		}
		CalculateCenter( pos, ActiveEnemyUnits.size() );
	}
	//ai->utility->Log( LOG_DEBUG, KNOWLEDGE, "Done! :-) " );
}

bool Battle::Contains( int unitID )
{
	map<int, UnitInformationContainer>::iterator unit;

	unit = ActiveEnemyUnits.find( unitID );
	if ( unit != ActiveEnemyUnits.end() )
		return true;

	unit = ActiveFriendlyUnits.find( unitID );
	if ( unit != ActiveFriendlyUnits.end() )
		return true;
	return false;
}

SAIFloat3 Battle::GetCenter()
{
	return Center;
}

void Battle::CalculateCenter( SAIFloat3 pos[], int size )
{
	SAIFloat3 center;
	center.x = 0;
	center.y = 0;
	center.z = 0;

	for ( int i = 0 ; i < size ; i++ )
	{
		center.x += pos[i].x;
		center.z += pos[i].z;
	}

	center.x /= size;
	center.z /= size;
	center.y = 200;


	if ( RadiusCircleID != -666 )
		ai->utility->RemoveGraphics( RadiusCircleID );
	RadiusCircleID = ai->utility->DrawCircle( center, Radius );
	Center = center;
}

float Battle::GetRadius()
{
	return Radius;
}



int Battle::GetNumberOfActiveUnits()
{
	return ActiveEnemyUnits.size() + ActiveFriendlyUnits.size();
}

int Battle::GetNumberOfDeadUnits()
{
	map<int, int>::iterator iter;
	int count = 0;
	for ( iter = DeadEnemyUnits.begin() ; iter != DeadEnemyUnits.end() ; iter++ )
		count += (*iter).second;

	for ( iter = DeadFriendlyUnits.begin() ; iter != DeadFriendlyUnits.end() ; iter++ )
		count += (*iter).second;
	return count;
}


int Battle::GetLastFrameOfActivity()
{
	return LastFrameOfActivity;
}

void Battle::RemoveUnit( int unitID )
{
	//ai->utility->Log( LOG_DEBUG, KNOWLEDGE, "UNIT REMOVED: %d", unit->GetUnitId() );
	map<int, UnitInformationContainer>::iterator it = ActiveFriendlyUnits.find( unitID );
	if ( it != ActiveFriendlyUnits.end() )
	{
		ActiveFriendlyUnits.erase( it );
		//ai->utility->Log( LOG_DEBUG, CHAT, "Removed a unit from a battle" );
	}
	else
	{
		it = ActiveEnemyUnits.find( unitID );
		if ( it != ActiveEnemyUnits.end() )
		{
			ActiveEnemyUnits.erase( it );
			//ai->utility->Log( LOG_DEBUG, CHAT, "Removed a unit from a battle" );
		}
	}
}

void Battle::Oldify()
{
	//labelling
	if(BattleLabel == RANDOM)//battlelabel was random, check if it might have been attack (range could have changed?	 
	{
		ai->utility->Log( LOG_DEBUG, KNOWLEDGE, "Random battle at end..");
		bool myattack = ai->knowledge->enemyInfo->baseInfo->IsBuildingInRange(Center, Radius);
		bool hisattack = ai->knowledge->selfInfo->baseInfo->IsBuildingInRange(Center, Radius);
		if(myattack)
		{
			ai->utility->Log( LOG_DEBUG, KNOWLEDGE, "Battle wasnt random, it was us attacking him!");
			BattleLabel = MYATTACK;
		}
		else if(hisattack)
		{
			ai->utility->Log( LOG_DEBUG, KNOWLEDGE, "Battle wasnt random, it was HIM attacking US! damn..");
			BattleLabel = HISATTACK;
		}
	}
	//was this just a scout thing?
	if(BattleLabel == MYATTACK)
	{
		if((ActiveFriendlyUnits.size() + DeadFriendlyUnits.size()) < 3)
		{
			ai->utility->Log( LOG_DEBUG, KNOWLEDGE, "Attack was just us scouting him");
			BattleLabel = MYSCOUT;
		}
	}
	else if(BattleLabel == HISATTACK)
	{
		if((ActiveEnemyUnits.size() + DeadEnemyUnits.size()) < 3)
		{
			ai->utility->Log( LOG_DEBUG, KNOWLEDGE, "Attack was just him scouting us");
			BattleLabel = HISSCOUT;
		}
	}
	ai->utility->RemoveGraphics( RadiusCircleID );
	ActiveEnemyUnits.clear();
	ActiveFriendlyUnits.clear();
	ai->utility->Log( LOG_DEBUG, KNOWLEDGE, "Final Battlelabel was: %d", BattleLabel);
}

void Battle::ToString()
{
	ai->utility->Log( LOG_DEBUG, KNOWLEDGE, "Active Friendly Units: %d\n--------------", ActiveFriendlyUnits.size() );
	for ( map<int, UnitInformationContainer>::iterator iter = ActiveFriendlyUnits.begin() ; iter != ActiveFriendlyUnits.end() ; iter++ )
	{
		if ( (*iter).second.def != NULL )
			ai->utility->Log( LOG_DEBUG, KNOWLEDGE, "UnitID: %d - %s", (*iter).first, (*iter).second.def->GetHumanName() );
		else ai->utility->Log( LOG_DEBUG, KNOWLEDGE, "UnitID: %d - unitdef unknown", (*iter).first );
	}

	ai->utility->Log( LOG_DEBUG, KNOWLEDGE, "Active Enemy Units: %d\n--------------", ActiveEnemyUnits.size() );
	for ( map<int, UnitInformationContainer>::iterator iter = ActiveEnemyUnits.begin() ; iter != ActiveEnemyUnits.end() ; iter++ )
	{
		if ( (*iter).second.def != NULL )
			ai->utility->Log( LOG_DEBUG, KNOWLEDGE, "UnitID: %d - %s", (*iter).first, (*iter).second.def->GetHumanName() );
		else ai->utility->Log( LOG_DEBUG, KNOWLEDGE, "UnitID: %d - unitdef unknown", (*iter).first );
	}

	vector<UnitDef*> unitDefs = ai->callback->GetUnitDefs();
	ai->utility->Log( LOG_DEBUG, KNOWLEDGE, "Dead friendly units: \n--------------" );
	for ( map<int, int>::iterator iter = DeadFriendlyUnits.begin() ; iter != DeadFriendlyUnits.end() ; iter++ )
	{
		for ( int i = 0 ; i < (int)unitDefs.size() ; i++ )
		{
			if ( unitDefs[i]->GetUnitDefId() == (*iter).first )
			{
				ai->utility->Log( LOG_DEBUG, KNOWLEDGE, "%s died %d times", unitDefs[i]->GetHumanName(), (*iter).second );
				break;
			}
		}
	}


	ai->utility->Log( LOG_DEBUG, KNOWLEDGE, "Dead enemy units: \n--------------" );
	for ( map<int, int>::iterator iter = DeadEnemyUnits.begin() ; iter != DeadEnemyUnits.end() ; iter++ )
	{
		for ( int i = 0 ; i < (int)unitDefs.size() ; i++ )
		{
			if ( unitDefs[i]->GetUnitDefId() == (*iter).first )
			{
				ai->utility->Log( LOG_DEBUG, KNOWLEDGE, "%s died %d times", unitDefs[i]->GetHumanName(), (*iter).second );
				break;
			}
		}
	}
}

void Battle::Update()
{
	list<int> enemyUnitsToErase;
	list<int> friendlyUnitsToErase;
	for ( map<int, UnitInformationContainer>::iterator iter = ActiveEnemyUnits.begin() ; iter != ActiveEnemyUnits.end() ; iter++ )
	{
		//If unit has moved outside the radius of the battle, remove it from the battle
		if ( ai->utility->EuclideanDistance(iter->second.pos, Center ) > Radius  )
			enemyUnitsToErase.push_back( (iter)->first );
		else if ( iter->second.def == NULL )
		{
			UnitDef* d = ai->knowledge->enemyInfo->armyInfo->GetUnitDef( iter->first );
			if ( d != NULL )
				iter->second.def = d;
		}
	}

	for ( map<int, UnitInformationContainer>::iterator iter = ActiveFriendlyUnits.begin() ; iter != ActiveFriendlyUnits.end() ; iter++ )
	{
		//If unit has moved outside the radius of the battle, remove it from the battle
		if ( ai->utility->EuclideanDistance(iter->second.pos, Center ) > Radius  )
			friendlyUnitsToErase.push_back( (iter)->first );
		else if ( iter->second.def == NULL )
		{
			UnitDef* d = ai->knowledge->enemyInfo->armyInfo->GetUnitDef( iter->first );
			if ( d != NULL )
				iter->second.def = d;
		}
	}

	for ( list<int>::iterator iter = friendlyUnitsToErase.begin() ; iter != friendlyUnitsToErase.end() ; iter++ )
	{
		ActiveFriendlyUnits.erase( *iter );
	}

	for ( list<int>::iterator iter = enemyUnitsToErase.begin() ; iter != enemyUnitsToErase.end() ; iter++ )
	{
		ActiveEnemyUnits.erase( *iter );
	}
}

int Battle::GetDuration()
{
	return LastFrameOfActivity - StartFrame;
}

void Battle::SomeoneDamaged()
{
	LastFrameOfActivity = ai->frame;
}
#include "Battle.h"

Battle::Battle( AIClasses* aiClasses, SAIFloat3 position )
{
	Center = position;
	Radius = INITIAL_BATTLE_RADIUS;
	ai = aiClasses;
	StartFrame = ai->frame;
	LastFrameOfActivity = ai->frame;
	RadiusCircleID = -666;
}


Battle::~Battle()
{
}


void Battle::UnitDied( Unit* u, bool enemy )
{
	LastFrameOfActivity = ai->frame;
	//ai->utility->Log( DEBUG, KNOWLEDGE, "UNIT DIED: %d unitdefID: %d", u->GetUnitId(), u->GetDef()->GetUnitDefId() );
	UnitDef* def = u->GetDef();
	map<int, int>::iterator iter;
	int unitID = u->GetUnitId();

	if ( enemy )
	{
		//ai->utility->Log( DEBUG, KNOWLEDGE, "dead unit is enemy unit" );
		DeadEnemyUnits[def->GetUnitDefId()] += 1;
		ActiveEnemyUnits.erase( unitID );
	}
	else
	{
		//ai->utility->Log( DEBUG, KNOWLEDGE, "dead unit is friendly unit" );
		DeadFriendlyUnits[def->GetUnitDefId()] += 1;
		ActiveFriendlyUnits.erase( unitID );
	}
}

void Battle::UnitEnteredBattle( Unit* u, bool enemy )
{
	UnitInformationContainer container;
	container.def = u->GetDef();
	container.pos = u->GetPos();
	
	int unitID = u->GetUnitId();

	ai->utility->Log( DEBUG, KNOWLEDGE, "UnitID %d entered battle: defID %d", unitID, container.def->GetUnitDefId() );
	if ( container.def->GetUnitDefId() == -1 )
	{
		ai->utility->Log( DEBUG, KNOWLEDGE, "defID was -1" );
		UnitDef* def = ai->knowledge->enemyInfo->armyInfo->GetUnitDef( unitID );
		if ( def->GetUnitDefId() != -1 )
		{
			container.def = def;
			ai->utility->Log( DEBUG, KNOWLEDGE, "Def updated!" );
		}
		ai->utility->Log( DEBUG, KNOWLEDGE, "Def not updated" );
	}
	LastFrameOfActivity = ai->frame;


	map<int, UnitInformationContainer>::iterator iter;
	if ( double distance = (ai->utility->EuclideanDistance( u->GetPos(), Center )) > Radius )
	{
		Radius = distance;
		ai->utility->Log(ALL, KNOWLEDGE, "New radius: %f", Radius );
	}
	if ( !enemy )
	{
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
		ActiveEnemyUnits[unitID] = container;

		SAIFloat3 pos[ActiveEnemyUnits.size()];

		int i = 0;
		for ( iter = ActiveEnemyUnits.begin() ; iter != ActiveEnemyUnits.end() ; iter++, i++ )
			pos[i] = (*iter).second.pos;
		CalculateCenter( pos, ActiveEnemyUnits.size() );
	}
	ai->utility->Log( DEBUG, KNOWLEDGE, "Done! :-) " );
}

bool Battle::Contains( Unit* u )
{
	map<int, UnitInformationContainer>::iterator unit;
	int unitID = u->GetUnitId();

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

void Battle::RemoveUnit( Unit* unit )
{
	//ai->utility->Log( DEBUG, KNOWLEDGE, "UNIT REMOVED: %d", unit->GetUnitId() );
	map<int, UnitInformationContainer>::iterator it = ActiveFriendlyUnits.find( unit->GetUnitId() );
	if ( it != ActiveFriendlyUnits.end() )
	{
		ActiveFriendlyUnits.erase( it );
		//ai->utility->Log( DEBUG, CHAT, "Removed a unit from a battle" );
	}
	else
	{
		it = ActiveEnemyUnits.find( unit->GetUnitId() );
		if ( it != ActiveEnemyUnits.end() )
		{
			ActiveEnemyUnits.erase( it );
			//ai->utility->Log( DEBUG, CHAT, "Removed a unit from a battle" );
		}
	}
}

void Battle::Oldify()
{
	ai->utility->RemoveGraphics( RadiusCircleID );
	ActiveEnemyUnits.clear();
	ActiveFriendlyUnits.clear();
}

void Battle::ToString()
{
	ai->utility->Log( DEBUG, KNOWLEDGE, "Active Friendly Units: %d\n--------------", ActiveFriendlyUnits.size() );
	for ( map<int, UnitInformationContainer>::iterator iter = ActiveFriendlyUnits.begin() ; iter != ActiveFriendlyUnits.end() ; iter++ )
	{
		if ( (*iter).second.def->GetUnitDefId() != -1 )
			ai->utility->Log( DEBUG, KNOWLEDGE, "UnitID: %d - %s", (*iter).first, (*iter).second.def->GetHumanName() );
		else ai->utility->Log( DEBUG, KNOWLEDGE, "UnitID: %d - unitdef unknown", (*iter).first );

		/* SUPER DEBUG
		Unit* u = Unit::GetInstance( ai->callback , (*iter).first );
		UnitDef* d = u->GetDef();
		ai->utility->Log( DEBUG, KNOWLEDGE, "Active friendly unitID %d and unitdefID %d", u->GetUnitId(), (*iter).second.def->GetUnitDefId() );
		if ( (*iter).second.def->GetUnitDefId() == -1 )
			ai->utility->Log( DEBUG, KNOWLEDGE, "Unit ID %d: UnitDef unknown", (*iter).first );
		else 
			ai->utility->Log( DEBUG, KNOWLEDGE, "Unit ID %d: %s", (*iter).first, (*iter).second.def->GetHumanName() );
			*/

	}

	ai->utility->Log( DEBUG, KNOWLEDGE, "Active Enemy Units: %d\n--------------", ActiveEnemyUnits.size() );
	for ( map<int, UnitInformationContainer>::iterator iter = ActiveEnemyUnits.begin() ; iter != ActiveEnemyUnits.end() ; iter++ )
	{
		if ( (*iter).second.def->GetUnitDefId() != -1 )
			ai->utility->Log( DEBUG, KNOWLEDGE, "UnitID: %d - %s", (*iter).first, (*iter).second.def->GetHumanName() );
		else ai->utility->Log( DEBUG, KNOWLEDGE, "UnitID: %d - unitdef unknown", (*iter).first );

		/* SUPER DEBUG
		Unit* u = Unit::GetInstance( ai->callback , (*iter).first );
		ai->utility->Log( DEBUG, KNOWLEDGE, "Active enemy unitID %d and unitdefID %d", u->GetUnitId(), (*iter).second.def->GetUnitDefId() );

		if ( (*iter).second.def->GetUnitDefId() == -1 )
			ai->utility->Log( DEBUG, KNOWLEDGE, "Unit ID %d: UnitDef unknown", (*iter).first );
		else 
			ai->utility->Log( DEBUG, KNOWLEDGE, "Unit ID %d: %s", (*iter).first, (*iter).second.def->GetHumanName() );
		*/
	}

	vector<UnitDef*> unitDefs = ai->callback->GetUnitDefs();
	ai->utility->Log( DEBUG, KNOWLEDGE, "Dead friendly units: \n--------------" );
	for ( map<int, int>::iterator iter = DeadFriendlyUnits.begin() ; iter != DeadFriendlyUnits.end() ; iter++ )
	{
		for ( int i = 0 ; i < unitDefs.size() ; i++ )
		{
			if ( unitDefs[i]->GetUnitDefId() == (*iter).first )
			{
				ai->utility->Log( DEBUG, KNOWLEDGE, "%s died %d times", unitDefs[i]->GetHumanName(), (*iter).second );
				break;
			}
		}
	}


	ai->utility->Log( DEBUG, KNOWLEDGE, "Dead enemy units: \n--------------" );
	for ( map<int, int>::iterator iter = DeadEnemyUnits.begin() ; iter != DeadEnemyUnits.end() ; iter++ )
	{
		for ( int i = 0 ; i < unitDefs.size() ; i++ )
		{
			if ( unitDefs[i]->GetUnitDefId() == (*iter).first )
			{
				ai->utility->Log( DEBUG, KNOWLEDGE, "%s died %d times", unitDefs[i]->GetHumanName(), (*iter).second );
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
		else if ( iter->second.def->GetUnitDefId() == -1 )
			iter->second.def = ai->knowledge->enemyInfo->armyInfo->GetUnitDef( iter->first );
	}

	for ( map<int, UnitInformationContainer>::iterator iter = ActiveFriendlyUnits.begin() ; iter != ActiveFriendlyUnits.end() ; iter++ )
	{
		//If unit has moved outside the radius of the battle, remove it from the battle
		if ( ai->utility->EuclideanDistance(iter->second.pos, Center ) > Radius  )
			friendlyUnitsToErase.push_back( (iter)->first );
		else if ( iter->second.def->GetUnitDefId() == -1 )
			iter->second.def = ai->knowledge->enemyInfo->armyInfo->GetUnitDef( iter->first );
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
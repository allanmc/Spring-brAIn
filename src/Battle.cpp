#include "Battle.h"

Battle::Battle( AIClasses* aiClasses, SAIFloat3 position )
{
	Center = position;
	ai = aiClasses;
}


Battle::~Battle()
{
}

void Battle::UpdateUnitPosition( Unit* u, bool enemy )
{
	map<int, SAIFloat3>::iterator unit;
	int unitID = u->GetUnitId();
	bool canInsert = true;
	if ( !enemy )
	{
		unit = ActiveFriendlyUnits.find( unitID );
		if ( unit == ActiveFriendlyUnits.end() )
			canInsert = false;
	}
	else 
	{
		unit = ActiveEnemyUnits.find( unitID );
		if ( unit == ActiveEnemyUnits.end() )
			canInsert = false;
	}
	if ( canInsert ) //If found, update the position of the unit
	{
		Unit* u = Unit::GetInstance( ai->callback, unitID );
		unit->second.x = u->GetPos().x;
		unit->second.y = u->GetPos().y;
		unit->second.z = u->GetPos().z;
	}
}


void Battle::UnitDied( Unit* u, bool enemy )
{
	UnitDef* def = u->GetDef();
	map<UnitDef*, int>::iterator iter;
	int unitID = u->GetUnitId();

	if ( enemy )
	{
		DeadEnemyUnits[def] = DeadEnemyUnits[def]+1;
		ActiveEnemyUnits.erase( unitID );
		ai->utility->ChatMsg( "Now %d %s enemy units in this battle have died!!", DeadEnemyUnits[def], def->GetHumanName() );
		ai->utility->ChatMsg( "%d enemy active units left in this battle", ActiveEnemyUnits.size() );
	}
	else
	{
		DeadFriendlyUnits[def] = DeadFriendlyUnits[def]+1;
		ActiveFriendlyUnits.erase( unitID );
		ai->utility->ChatMsg( "Now %d %s friendly units in this battle have died!!", DeadFriendlyUnits[def], def->GetHumanName() );
		ai->utility->ChatMsg( "%d active friendly units left in this battle", ActiveFriendlyUnits.size() );
	}
}

void Battle::UnitEnteredBattle( Unit* u, bool enemy )
{
	int unitID = u->GetUnitId();
	if ( !enemy )
	{
		ActiveFriendlyUnits.insert( make_pair( unitID, springai::Unit::GetInstance( ai->callback, unitID )->GetPos() ) );	
	}
	else ActiveEnemyUnits.insert( make_pair( unitID, springai::Unit::GetInstance( ai->callback, unitID )->GetPos() ) );
}

bool Battle::Contains( Unit* u )
{
	map<int, SAIFloat3>::iterator unit;
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
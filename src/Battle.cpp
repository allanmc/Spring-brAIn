#include "Battle.h"

Battle::Battle( AIClasses* aiClasses, SAIFloat3 position )
{
	Center = position;
	ai = aiClasses;
}


Battle::~Battle()
{
}

void Battle::UpdateUnitPosition( int unitID, bool enemy )
{
	map<int, SAIFloat3>::iterator unit;
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


void Battle::UnitDied( int unitID, bool enemy )
{
}


void Battle::UnitEnteredBattle( int unitID, bool enemy )
{
	if ( !enemy )
	{
		ActiveFriendlyUnits.insert( make_pair( unitID, springai::Unit::GetInstance( ai->callback, unitID )->GetPos() ) );	
	}
	else ActiveEnemyUnits.insert( make_pair( unitID, springai::Unit::GetInstance( ai->callback, unitID )->GetPos() ) );
}

bool Battle::Contains( int unitID )
{
	map<int, SAIFloat3>::iterator unit;
	
	unit = ActiveEnemyUnits.find( unitID );
	if ( unit != ActiveEnemyUnits.end() )
		return true;

	unit = ActiveFriendlyUnits.find( unitID );
	if ( unit != ActiveFriendlyUnits.end() )
		return true;

	return false;
}
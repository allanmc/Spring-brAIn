#include "Battle.h"

Battle::Battle( AIClasses* aiClasses, SAIFloat3 position )
{
	Center = position;
	Radius = INITIAL_BATTLE_RADIUS;
	ai = aiClasses;
	LastFrameOfActivity = ai->frame;
	RadiusCircleID = -666;
}


Battle::~Battle()
{
	ai->utility->Log( ALL, KNOWLEDGE, "Battle destructor Radius Circle ID: %d", RadiusCircleID );
	ai->utility->RemoveGraphics( RadiusCircleID );
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
	LastFrameOfActivity = ai->frame;
	UnitDef* def = u->GetDef();
	map<int, int>::iterator iter;
	int unitID = u->GetUnitId();

	if ( enemy )
	{
		DeadEnemyUnits[def->GetUnitDefId()] += 1;
		ActiveEnemyUnits.erase( unitID );
		ai->utility->ChatMsg( "Now %d %s enemy units in this battle have died!!", DeadEnemyUnits[def->GetUnitDefId()], def->GetHumanName() );
		ai->utility->ChatMsg( "%d enemy active units left in this battle", ActiveEnemyUnits.size() );
	}
	else
	{
		DeadFriendlyUnits[def->GetUnitDefId()] += 1;
		ActiveFriendlyUnits.erase( unitID );
		ai->utility->Log(ALL, KNOWLEDGE, "Now %d %s friendly units in this battle have died!!", DeadFriendlyUnits[def->GetUnitDefId()], def->GetHumanName() );
		ai->utility->Log( ALL, KNOWLEDGE, "%d active friendly units left in this battle", ActiveFriendlyUnits.size() );
	}
}

void Battle::UnitEnteredBattle( Unit* u, bool enemy )
{
	LastFrameOfActivity = ai->frame;

	int unitID = u->GetUnitId();
	map<int, SAIFloat3>::iterator iter;
	if ( double distance = (ai->utility->EuclideanDistance( u->GetPos(), Center )) > Radius )
	{
		Radius = distance;
		ai->utility->Log(ALL, KNOWLEDGE, "New radius: %f", Radius );
	}
	if ( !enemy )
	{
		ActiveFriendlyUnits[unitID] = springai::Unit::GetInstance( ai->callback, unitID )->GetPos();

		SAIFloat3 pos[ActiveFriendlyUnits.size()];

		int i = 0;
		for ( iter = ActiveFriendlyUnits.begin() ; iter != ActiveFriendlyUnits.end() ; iter++, i++ )
		{
			pos[i] = (*iter).second;
		}
		CalculateCenter( pos, ActiveFriendlyUnits.size() );
	}
	else
	{
		ActiveEnemyUnits[unitID] = springai::Unit::GetInstance( ai->callback, unitID )->GetPos();

		SAIFloat3 pos[ActiveEnemyUnits.size()];

		int i = 0;
		for ( iter = ActiveEnemyUnits.begin() ; iter != ActiveEnemyUnits.end() ; iter++, i++ )
			pos[i] = (*iter).second;
		CalculateCenter( pos, ActiveEnemyUnits.size() );
	}
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
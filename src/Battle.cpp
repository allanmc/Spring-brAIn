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
	ai->utility->Log( DEBUG, KNOWLEDGE, "UNIT DIED: %d unitdefID: %d", u->GetUnitId(), u->GetDef()->GetUnitDefId() );
	UnitDef* def = u->GetDef();
	map<int, int>::iterator iter;
	int unitID = u->GetUnitId();

	if ( enemy )
	{
		DeadEnemyUnits[def->GetUnitDefId()] += 1;
		ActiveEnemyUnits.erase( unitID );
	}
	else
	{
		DeadFriendlyUnits[def->GetUnitDefId()] += 1;
		ActiveFriendlyUnits.erase( unitID );
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

void Battle::RemoveUnit( Unit* unit )
{
	ai->utility->Log( DEBUG, KNOWLEDGE, "UNIT REMOVED: %d", unit->GetUnitId() );
	map<int, SAIFloat3>::iterator it = ActiveFriendlyUnits.find( unit->GetUnitId() );
	if ( it != ActiveFriendlyUnits.end() )
	{
		ActiveFriendlyUnits.erase( it );
		ai->utility->Log( DEBUG, CHAT, "Removed a unit from a battle" );
	}
	else
	{
		it = ActiveEnemyUnits.find( unit->GetUnitId() );
		if ( it != ActiveEnemyUnits.end() )
		{
			ActiveEnemyUnits.erase( it );
			ai->utility->Log( DEBUG, CHAT, "Removed a unit from a battle" );
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
	for ( map<int, SAIFloat3>::iterator iter = ActiveFriendlyUnits.begin() ; iter != ActiveFriendlyUnits.end() ; iter++ )
	{
		int i = (*iter).first;
		ai->utility->Log( DEBUG, KNOWLEDGE, "PIKKEMAND %d", i );
		Unit* u = Unit::GetInstance( ai->callback , (*iter).first );
	
		ai->utility->Log( DEBUG, KNOWLEDGE, "RØVSVED" );
		UnitDef* d = u->GetDef();
		if ( d == NULL )
			ai->utility->Log( DEBUG, KNOWLEDGE, "ANAL KLØE" );
		ai->utility->Log( DEBUG, KNOWLEDGE, "HOMOSEX %d", d->GetUnitDefId() );

		const char* s = d->GetHumanName();
		ai->utility->Log( DEBUG, KNOWLEDGE, "DILLERBUMMELUM" );


		ai->utility->Log( DEBUG, KNOWLEDGE, "Unit ID %d: %s", (*iter).first, springai::Unit::GetInstance(ai->callback, (*iter).first )->GetDef()->GetHumanName() );
	}

	ai->utility->Log( DEBUG, KNOWLEDGE, "Active Enemy Units: %d\n--------------", ActiveEnemyUnits.size() );
	for ( map<int, SAIFloat3>::iterator iter = ActiveEnemyUnits.begin() ; iter != ActiveEnemyUnits.end() ; iter++ )
	{
		int i = (*iter).first;
		ai->utility->Log( DEBUG, KNOWLEDGE, "PIKKEMAND %d", i );
		Unit* u = Unit::GetInstance( ai->callback , (*iter).first );
		ai->utility->Log( DEBUG, KNOWLEDGE, "RØVSVED" );
		UnitDef* d = u->GetDef();
		ai->utility->Log( DEBUG, KNOWLEDGE, "HOMOSEX %d", d->GetUnitDefId() );


		if ( d == NULL )
			ai->utility->Log( DEBUG, KNOWLEDGE, "ANAL KLØE" );
		const char* s = d->GetHumanName();
		ai->utility->Log( DEBUG, KNOWLEDGE, "DILLERBUMMELUM" );

		ai->utility->Log( DEBUG, KNOWLEDGE, "Unit ID %d: %s", (*iter).first, springai::Unit::GetInstance(ai->callback, (*iter).first )->GetDef()->GetHumanName() );
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
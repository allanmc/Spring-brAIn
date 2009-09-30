#include "BattlesInfo.h"


using namespace brainSpace;
using namespace springai;

int LAST_NUMBER_OF_BATTLES = 0;

BattlesInfo::BattlesInfo( AIClasses* aiClasses )
{
	ai = aiClasses;
	LastUpdateFrame = ai->frame;
}

BattlesInfo::~BattlesInfo()
{
}


//1a: Find the battle which the friendly unit is a part of
//1b: Add the attacker to the battle
//2a: The friendly unit was not a part of any ongoing battle, so try and add it to the nearest battle
//2b: If that battle did not already contain the attacker, add it to the battle
//3a: No battles were taking place, so add both units to a new Battle object
void BattlesInfo::UnitDamaged( Unit* friendlyUnit, Unit* attackingUnit )
{
	Battle* b = FindBattleContaining( friendlyUnit );

	if ( b != NULL )
	{
		if ( !b->Contains( attackingUnit ) )
			b->UnitEnteredBattle( attackingUnit, true );
		return;
	}
	else
	{
		b = FindNearestBattle( friendlyUnit->GetPos() );
		if ( b != NULL )
		{
			b->UnitEnteredBattle( friendlyUnit, false );
			if ( !b->Contains( attackingUnit ) )
				b->UnitEnteredBattle( attackingUnit, true );
			return;
		}
		//The unit is not a part of any battles, and there are no battles nearby so make a new Battle object.

		b = new Battle( ai, friendlyUnit->GetPos() );
		b->UnitEnteredBattle( friendlyUnit, false );
		b->UnitEnteredBattle( attackingUnit, true );
		CurrentBattles.push_back( b );
		ai->utility->Log( ALL, KNOWLEDGE, "Units entered a new battle: Id %d and %d ", friendlyUnit->GetUnitId(), attackingUnit->GetUnitId() );
	}

	ai->utility->Log( ALL, KNOWLEDGE, "Num battles: %d", CurrentBattles.size() );
	list<Battle*>::iterator iter;
	int i = 0;
	for ( iter = CurrentBattles.begin() ; iter != CurrentBattles.end() ; iter++, i++ )
	{
		int a = (*iter)->GetNumberOfActiveUnits();
		int d = (*iter)->GetNumberOfDeadUnits();
		ai->utility->Log( ALL, KNOWLEDGE, "Number of units in battle %d: Dead %d Alive: %d", i, d, a );
	}
}


void BattlesInfo::UnitDestroyed( Unit* deadFriendlyUnit, Unit* attackingUnit  )
{
	Battle* b = FindBattleContaining( deadFriendlyUnit );
	if ( b != NULL )
	{
		b->UnitDied( deadFriendlyUnit, false );
	}
}

void BattlesInfo::EnemyDestroyed( Unit* deadEnemyUnit, Unit* attackingUnit )
{
	Battle* b = FindBattleContaining( deadEnemyUnit );
	if ( b != NULL )
	{
		b->UnitDied( deadEnemyUnit, true );
	}
}

void BattlesInfo::EnemyDamaged( int unitID )
{
}

void BattlesInfo::Update ( int frame )
{
	if ( LAST_NUMBER_OF_BATTLES != CurrentBattles.size() )
		ai->utility->Log( ALL, KNOWLEDGE, "BattleInfo: Number of battles %d", CurrentBattles.size() );
	
	LAST_NUMBER_OF_BATTLES = CurrentBattles.size();
		
	LastUpdateFrame = frame;
	/** Delete any inactive battles **/
	for ( list<Battle*>::iterator iter = CurrentBattles.begin() ; iter != CurrentBattles.end() ; iter++ )
	{
		if ((*iter)->GetLastFrameOfActivity() + BATTLE_TIMEOUT < LastUpdateFrame )
		{
			list<Battle*>::iterator it2 = iter;
			if ( iter == CurrentBattles.begin() )
				iter++;
			else 
				iter--;
			CurrentBattles.erase( it2 );
			ai->utility->Log( ALL, KNOWLEDGE, "Inactive battle erased" );
		}
	}
}

Battle* BattlesInfo::FindBattleContaining( Unit* u )
{
	list<Battle*>::iterator iter;

	for ( iter = CurrentBattles.begin() ; iter != CurrentBattles.end() ; iter++ )
		if ( (*iter)->Contains( u ) )
			return *iter;
	return NULL;
}

Battle* BattlesInfo::FindNearestBattle( SAIFloat3 pos )
{
	list<Battle*>::iterator iter;

	for ( iter = CurrentBattles.begin() ; iter != CurrentBattles.end() ; iter++ )
		if ( ai->utility->EuclideanDistance( (*iter)->GetCenter(), pos ) < (*iter)->GetRadius() )
		{
			return *iter;
		}
		return NULL;
}

#include "BattlesInfo.h"


using namespace brainSpace;
using namespace springai;

BattlesInfo::BattlesInfo( AIClasses* aiClasses )
{
	ai = aiClasses;
}

BattlesInfo::~BattlesInfo()
{
}

void BattlesInfo::UnitDamaged( Unit* friendlyUnit, Unit* attackingUnit )
{
	Battle* b = FindBattleContaining( friendlyUnit );

	if ( b != NULL )
	{
		ai->utility->ChatMsg( "UnitID: %d, AttackerID: %d", friendlyUnit->GetUnitId(), attackingUnit->GetUnitId() );
		if ( !b->Contains( attackingUnit ) )
			b->UnitEnteredBattle( attackingUnit, true );
		return;
	}
	else
	{
		ai->utility->ChatMsg( "UnitID: %d was not a part of any battle", friendlyUnit->GetUnitId() );
		b = FindNearestBattle( friendlyUnit->GetPos() );
		if ( b != NULL )
		{
			ai->utility->ChatMsg( "Nearby battle found" );
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
		ai->utility->ChatMsg( "Units entered a new battle: Id %d and %d ", friendlyUnit->GetUnitId(), attackingUnit->GetUnitId() );
	}
}


void BattlesInfo::UnitDestroyed( Unit* friendlyUnit, Unit* attackingUnit  )
{
	Battle* b = FindBattleContaining( friendlyUnit );
	if ( b != NULL )
	{
		b->UnitDied( friendlyUnit, false );
	}
}

void BattlesInfo::EnemyDestroyed(int unitID)
{
}

void BattlesInfo::EnemyDamaged( int unitID )
{
}

void BattlesInfo::Update ( int frame )
{
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
		if ( ai->utility->EuclideanDistance( (*iter)->GetCenter(), pos ) < BATTLE_RADIUS )
			return *iter;
	return NULL;
}
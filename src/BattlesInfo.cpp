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

void BattlesInfo::UnitDamaged( int unitID, int attackerID )
{
	Battle* b = FindBattleContaining( unitID );
	if ( b != NULL )
	{
		if ( !b->Contains( attackerID ) )
		{
			b->UnitEnteredBattle( attackerID, true );
		}
		return;
	}
	//The unit is not a part of any battles, so make a new Battle object.
	Unit* u = Unit::GetInstance( ai->callback, unitID );
	b = new Battle( ai, u->GetPos() );
	b->UnitEnteredBattle( unitID, false );
	b->UnitEnteredBattle( attackerID, true );
	CurrentBattles.push_back( b );
	//ai->utility->ChatMsg( "Units entered a new battle: Id %d and %d ", unitID, attackerID );
}


void BattlesInfo::UnitDestroyed( int unitID, int attackerID  )
{
	UnitDef* def = Unit::GetInstance( ai->callback, unitID )->GetDef();
	Battle* b = FindBattleContaining( unitID );
	if ( b != NULL )
	{
		b->UnitDied( unitID, false );
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

Battle* BattlesInfo::FindBattleContaining( int unitID )
{
	list<Battle*>::iterator iter;

	for ( iter = CurrentBattles.begin() ; iter != CurrentBattles.end() ; iter++ )
		if ( (*iter)->Contains( unitID ) )
			return *iter;
	return NULL;
}
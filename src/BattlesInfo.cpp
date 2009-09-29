#include "BattlesInfo.h"


using namespace brainSpace;
using namespace springai;

BattlesInfo::BattlesInfo( AIClasses* aiClasses )
{
	ai = ai;
}

BattlesInfo::~BattlesInfo()
{
}

void BattlesInfo::UnitDamaged( int unitID, int attackerID )
{
	list<Battle*>::iterator iter;

	for ( iter = CurrentBattles.begin() ; iter != CurrentBattles.end() ; iter++ )
	{
		if ( (*iter)->Contains( unitID ) )
		{
			if ( !(*iter)->Contains( attackerID ) )
			{
				(*iter)->UnitEnteredBattle( attackerID, true );
			}
			return;
		}
	}

	//The unit is not a part of any battles, so make a new Battle object.
	Unit* u = Unit::GetInstance( ai->callback, unitID );
	Battle* b = new Battle( ai, u->GetPos() );
	b->UnitEnteredBattle( unitID, false );
	b->UnitEnteredBattle( attackerID, true );
	CurrentBattles.push_back( b );
	//ai->utility->ChatMsg( "Units entered a new battle: Id %d and %d ", unitID, attackerID );
}


void BattlesInfo::UnitDestroyed( int unitID )
{

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
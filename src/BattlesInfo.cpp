#include "BattlesInfo.h"


using namespace brainSpace;
using namespace springai;

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
	SomeoneDamaged( friendlyUnit, attackingUnit );
}


void BattlesInfo::UnitDestroyed( Unit* deadFriendlyUnit, Unit* attackingUnit  )
{
	Battle* b = FindBattleContaining( deadFriendlyUnit );
	if ( b != NULL )
	{
		b->UnitDied( deadFriendlyUnit, false );
	}
	CleanupAfterSomeoneDied( deadFriendlyUnit );
}

void BattlesInfo::EnemyDestroyed( Unit* deadEnemyUnit, Unit* attackingUnit )
{
	ai->utility->Log( DEBUG, KNOWLEDGE, "røv!!!!!!" );
	Battle* b = FindBattleContaining( deadEnemyUnit );
	ai->utility->Log( DEBUG, KNOWLEDGE, "røv2!!!!!!" );
	if ( b != NULL )
	{ai->utility->Log( DEBUG, KNOWLEDGE, "initram" );
	
		b->UnitDied( deadEnemyUnit, true );
		ai->utility->Log( DEBUG, KNOWLEDGE, "nosser" );
	
	}
	CleanupAfterSomeoneDied( deadEnemyUnit );
	ai->utility->Log( DEBUG, KNOWLEDGE, "så skal der noget debug ud" );
	
}

void BattlesInfo::EnemyDamaged( Unit* attacker, Unit* enemy )
{
	SomeoneDamaged( attacker, enemy );
}

void BattlesInfo::Update ( int frame )
{
	LastUpdateFrame = frame;
	/** Erase any inactive battles and copy them to the OldBattles list **/
	for ( list<Battle*>::iterator iter = CurrentBattles.begin() ; iter != CurrentBattles.end() ; iter++ )
	{
		if ((*iter)->GetLastFrameOfActivity() + BATTLE_TIMEOUT < LastUpdateFrame )
		{
			(*iter)->Oldify();
			OldBattles.push_back( *iter );
			iter = CurrentBattles.erase( iter );
			ai->utility->Log( DEBUG, KNOWLEDGE, "Inactive battle erased" );
		}
	}

		ai->utility->Log( DEBUG, KNOWLEDGE, "\nPRINTING UPDATED BATTLE INFO: frame %d\n", frame );
		ai->utility->Log( DEBUG, KNOWLEDGE, "\n\nNum active battles %d\n==============", CurrentBattles.size() );
		int i = 0;
		for ( list<Battle*>::iterator iter = CurrentBattles.begin() ; iter != CurrentBattles.end() ; iter++, i++ )
		{
			ai->utility->Log( DEBUG, KNOWLEDGE, "Battle number %d", i );
			(*iter)->ToString();
		}

		i = 0;
		ai->utility->Log( DEBUG, KNOWLEDGE, "\n\nNum old battles %d\n==============", OldBattles.size() );
		for ( list<Battle*>::iterator iter = OldBattles.begin() ; iter != OldBattles.end() ; iter++, i++ )
		{
			ai->utility->Log( DEBUG, KNOWLEDGE, "Battle number %d", i );
			(*iter)->ToString();
		}
		ai->utility->Log( DEBUG, KNOWLEDGE, "\nDONE PRINTING UPDATED BATTLE INFO\n" );

}

Battle* BattlesInfo::FindBattleContaining( Unit* u )
{
	for ( list<Battle*>::iterator iter = CurrentBattles.begin() ; iter != CurrentBattles.end() ; iter++ )
		if ( (*iter)->Contains( u ) )
			return *iter;
	return NULL;
}

Battle* BattlesInfo::FindNearestBattle( SAIFloat3 pos )
{
	for ( list<Battle*>::iterator iter = CurrentBattles.begin() ; iter != CurrentBattles.end() ; iter++ )
		if ( ai->utility->EuclideanDistance( (*iter)->GetCenter(), pos ) < (*iter)->GetRadius()+RADIUS_BUFFER )
		{
			return *iter;
		}
		return NULL;
}


void BattlesInfo::SomeoneDamaged( Unit* our, Unit* their )
{
	Battle* b = FindBattleContaining( our );

	if ( b != NULL )
	{
		if ( !b->Contains( their ) )
			b->UnitEnteredBattle( their, true );
		return;
	}
	else
	{
		b = FindNearestBattle( our->GetPos() );
		if ( b != NULL )
		{
			b->UnitEnteredBattle( our, false );
			if ( !b->Contains( their ) )
				b->UnitEnteredBattle( their, true );
			return;
		}
		//The unit is not a part of any battles, and there are no battles nearby so make a new Battle object.

		b = new Battle( ai, our->GetPos() );
		b->UnitEnteredBattle( our, false );
		b->UnitEnteredBattle( their, true );
		CurrentBattles.push_back( b );
	}

	int i = 0;
	for ( list<Battle*>::iterator iter = CurrentBattles.begin() ; iter != CurrentBattles.end() ; iter++, i++ )
	{
		int a = (*iter)->GetNumberOfActiveUnits();
		int d = (*iter)->GetNumberOfDeadUnits();
	}
}

void BattlesInfo::CleanupAfterSomeoneDied( Unit* unitToCleanup )
{
	for ( list<Battle*>::iterator iter = CurrentBattles.begin() ; iter != CurrentBattles.end() ; iter++ )
	{
		if ( (*iter)->Contains( unitToCleanup ) )
		{
			(*iter)->RemoveUnit( unitToCleanup );
		}
	}
}

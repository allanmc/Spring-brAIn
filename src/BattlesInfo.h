#ifndef _BRAINSPACE_BATTLESINFO_H
#define _BRAINSPACE_BATTLESINFO_H

#include "global.h"
#include "Battle.h"

using namespace std;
using namespace springai;

namespace brainSpace
{
	///Keeps track of both current and prior battles.
	class BattlesInfo
	{
	public:
		BattlesInfo( AIClasses* aiClasses );

		virtual ~BattlesInfo();

		void UnitDamaged( int friendlyUnit, int attackingUnit );
		
		void UnitDestroyed( int deadFriendlyUnit, int attackingUnit );
		
		void EnemyDestroyed( int deadEnemyUnit , int attackingUnit );

		void EnemyDamaged( int attacker, int enemy );

		void Update ( int frame );

		int NumberOfBattlesInArea( int timePeriod, CBoundingBox box );

	protected:

	private:

		void SomeoneDamaged( int our, int their );
		void CleanupAfterSomeoneDied(int unitID );
		Battle* FindBattleContaining( int unitID );
		Battle* FindNearestBattle( SAIFloat3 pos );
		
		
		AIClasses* ai;
		
		int LastUpdateFrame;

		list<Battle*> CurrentBattles;
		list<Battle*> OldBattles;

	};
}

#endif

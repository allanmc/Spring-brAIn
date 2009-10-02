#ifndef _BRAINSPACE_BATTLESINFO_H
#define _BRAINSPACE_BATTLESINFO_H

#include "global.h"
#include "Battle.h"

using namespace std;
using namespace springai;

namespace brainSpace
{
	class BattlesInfo
	{
	public:
		BattlesInfo( AIClasses* aiClasses );

		virtual ~BattlesInfo();

		void UnitDamaged( Unit* friendlyUnit, Unit* attackingUnit );
		
		void UnitDestroyed( Unit* deadFriendlyUnit, Unit* attackingUnit );
		
		void EnemyDestroyed( Unit* deadEnemyUnit , Unit* attackingUnit );

		void EnemyDamaged( Unit* attacker, Unit* enemy );

		void Update ( int frame );

		int NumberOfBattlesInArea( int timePeriod, CBoundingBox box );

	protected:

	private:

		void SomeoneDamaged( Unit* our, Unit* their );
		void CleanupAfterSomeoneDied( Unit* unitToCleanup );
		Battle* FindBattleContaining( Unit* unit );
		Battle* FindNearestBattle( SAIFloat3 pos );
		
		
		AIClasses* ai;
		
		int LastUpdateFrame;

		list<Battle*> CurrentBattles;
		list<Battle*> OldBattles;

	};
}

#endif

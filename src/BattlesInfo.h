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

		void EnemyDamaged( int unitID );

		void Update ( int frame );

	protected:

	private:

		int LastUpdateFrame;
		Battle* FindBattleContaining( Unit* unit );
		Battle* FindNearestBattle( SAIFloat3 pos );
		
		AIClasses* ai;

		list<Battle*> CurrentBattles;
		list<Battle*> OldBattles;

	};
}

#endif

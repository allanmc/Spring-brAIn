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

		void UnitDamaged( int unitID, int attackerID );
		
		void UnitDestroyed( int unitID, int attackerID );
		
		void EnemyDestroyed( int unitID );

		void EnemyDamaged( int unitID );

		void Update ( int frame );

	protected:

	private:

		Battle* FindBattleContaining( int unitID );


		AIClasses* ai;

		list<Battle*> CurrentBattles;
		list<Battle*> OldBattles;

	};
}

#endif

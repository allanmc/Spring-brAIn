#ifndef _BRAINSPACE_BATTLE_H
#define _BRAINSPACE_BATTLE_H

#include "global.h"

namespace brainSpace
{
	class Battle
	{
	public:
		Battle( AIClasses* aiClasses, SAIFloat3 pos );
		virtual ~Battle();


		void UpdateUnitPosition( int unitID, bool enemy );

		void UnitDied( int unitID, bool enemy );

		void UnitEnteredBattle( int unitID, bool enemy );

		bool Contains( int unitID );

	private:
		map<UnitDef*, int> DeadFriendlyUnits;
		map<UnitDef*, int> DeadEnemyUnits;

		//UnitID, Position
		map<int, SAIFloat3> ActiveFriendlyUnits;
		map<int, SAIFloat3> ActiveEnemyUnits;


		SAIFloat3 Center;
		float Radius;

		AIClasses* ai;
	};
};

#endif
#ifndef _BRAINSPACE_BATTLE_H
#define _BRAINSPACE_BATTLE_H

#define BATTLE_RADIUS 150

#include "global.h"

namespace brainSpace
{
	class Battle
	{
	public:
		Battle( AIClasses* aiClasses, SAIFloat3 pos );
		virtual ~Battle();


		void UpdateUnitPosition( Unit* u, bool enemy );

		void UnitDied( Unit* u, bool enemy );

		void UnitEnteredBattle( Unit* u, bool enemy );

		bool Contains( Unit* u );

		SAIFloat3 GetCenter();

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
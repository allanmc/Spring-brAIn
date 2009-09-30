#ifndef _BRAINSPACE_BATTLE_H
#define _BRAINSPACE_BATTLE_H

#define INITIAL_BATTLE_RADIUS 500
#define BATTLE_TIMEOUT 400

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

		float GetRadius();

		int GetLastFrameOfActivity();

		/**DEBUG**/
		int GetNumberOfActiveUnits();
		/**DEBUG**/
		int GetNumberOfDeadUnits();

	private:

		int LastFrameOfActivity;
		int RadiusCircleID;

		void CalculateCenter( SAIFloat3 pos[], int size );

		//UnitDefID, numberOfUnits
		map<int, int> DeadFriendlyUnits;
		map<int, int> DeadEnemyUnits;

		//UnitID, Position
		map<int, SAIFloat3> ActiveFriendlyUnits;
		map<int, SAIFloat3> ActiveEnemyUnits;

		SAIFloat3 Center;
		float Radius;

		AIClasses* ai;
	};
};

#endif
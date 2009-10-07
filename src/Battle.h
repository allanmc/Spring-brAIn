#ifndef _BRAINSPACE_BATTLE_H
#define _BRAINSPACE_BATTLE_H

#define INITIAL_BATTLE_RADIUS 500
#define RADIUS_BUFFER 300
#define BATTLE_TIMEOUT 400

#include "global.h"

namespace brainSpace
{

	struct UnitInformationContainer
	{
		SAIFloat3 pos;
		UnitDef* def;
	};

	enum BattleLabels {
		HISATTACK	= 0,
		MYATTACK	= 1,
		RANDOM		= 2,
		HISSCOUT	= 3,
		MYSCOUT		= 4,
	};

	class Battle
	{
	public:
		Battle( AIClasses* aiClasses, SAIFloat3 pos );

		virtual ~Battle();

		void UnitDied( Unit* u, bool enemy );

		void UnitEnteredBattle( Unit* u, bool enemy );

		bool Contains( Unit* u );

		SAIFloat3 GetCenter();

		float GetRadius();

		int GetLastFrameOfActivity();

		void RemoveUnit( Unit* unit );

		void Update();

		int GetDuration();

		/** This method performs stuff that is necessary when the battle goes from being 
		  * active to being inactive, i.e. when no units in it have been in battle for a while
		 **/
		void Oldify();

		void ToString();


		/**DEBUG**/
		int GetNumberOfActiveUnits();
		/**DEBUG**/
		int GetNumberOfDeadUnits();

	private:

		int StartFrame;
		int LastFrameOfActivity;
		int RadiusCircleID;
		int BattleLabel;

		void CalculateCenter( SAIFloat3 pos[], int size );

		//UnitDefID, numberOfUnits
		map<int, int> DeadFriendlyUnits;
		map<int, int> DeadEnemyUnits;

		//UnitID, Position
		map<int, UnitInformationContainer> ActiveFriendlyUnits;
		map<int, UnitInformationContainer> ActiveEnemyUnits;

		SAIFloat3 Center;
		float Radius;

		AIClasses* ai;
	};
};

#endif
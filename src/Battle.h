#ifndef _BRAINSPACE_BATTLE_H
#define _BRAINSPACE_BATTLE_H

#define INITIAL_BATTLE_RADIUS 500
#define RADIUS_BUFFER 300
#define BATTLE_TIMEOUT 400

#include "types.h"

namespace brainSpace
{




	///A class used to store information about a unit.
	/**
	* Used to make sure we keep the information when the unit moves out of LOS.
	*/
	typedef struct UnitInformationContainer
	{
		SAIFloat3 pos;
		springai::UnitDef* def;

		UnitInformationContainer()
		{
			def = NULL;
		}
	}UnitInformationContainer;

	///The ways we can label a battle
	enum BattleLabels {
		HISATTACK	= 0,
		MYATTACK	= 1,
		RANDOM		= 2,
		HISSCOUT	= 3,
		MYSCOUT		= 4,
	};

	///A class that keeps all nessasary information on a battle
	class Battle
	{
	public:
		Battle( AIClasses* aiClasses, SAIFloat3 pos );

		virtual ~Battle();

		void UnitDied( int unitID, bool enemy );

		void UnitEnteredBattle(int unitID, bool enemy );

		bool Contains( int unitID );

		SAIFloat3 GetCenter();

		float GetRadius();

		int GetLastFrameOfActivity();

		void RemoveUnit( int unitID );

		void Update();

		int GetDuration();

		void SomeoneDamaged();

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
		std::map<int, int> DeadFriendlyUnits;
		std::map<int, int> DeadEnemyUnits;

		//UnitID, Position
		std::map<int, UnitInformationContainer> ActiveFriendlyUnits;
		std::map<int, UnitInformationContainer> ActiveEnemyUnits;

		SAIFloat3 Center;
		float Radius;

		AIClasses* ai;
	};
};

#endif
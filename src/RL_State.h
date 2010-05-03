#ifndef _BRAINSPACE_RL_STATE_H
#define _BRAINSPACE_RL_STATE_H

#include "types.h"
#include "RL_Action.h"
#include "MilitaryUnitGroup.h"
#include "BattleFileReader.h"
#include "RL_Q.h"
using namespace springai;
using namespace std;

#define MEX_GROUP_TOLERANCE 400.0f

namespace brainSpace {

	struct AIClasses;
	class RL_State
	{
	public:
		RL_State();
		RL_State(AIClasses* aiClasses, MilitaryUnitGroup* currentGroup, std::vector<QStateVar> stateVars, vector< pair<int, SAIFloat3> > mexCluster, RL_Q* valueFunction );
		virtual ~RL_State();
		int GetID();
		std::vector<RL_Action*> GetActions();
		void DeleteAction(int actionID);
		bool IsTerminal();
		bool operator==(const RL_State &other) const;
		RL_State & operator=(const RL_State &rhs);
		
	protected:
		bool terminal;
		unsigned int ID;
		std::vector<RL_Action*> Actions;
		AIClasses* ai;
		BattleFileReader* Reader;

	private:

		int GetVisitsTo( int stateID );

		
		MilitaryUnitGroup* NearestEnemyGroup;
		Unit* NearestEnemySolar;
		Unit* NearestEnemyMex;
		Unit* NearestEnemyWindGenerator;
		Unit* EnemyCommander;


		
	};
}

#endif

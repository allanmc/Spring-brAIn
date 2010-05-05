#ifndef _BRAINSPACE_RL_H
#define _BRAINSPACE_RL_H

#define QBFILE_VERSION 2

#define PRINT_REWARD false

#define USE_QSMDP false
#define USE_RS_TIME false
#define USE_Q_LAMBDA false
#define LAMBDA 0.95f
#define Q_LAMBDA_THRESHOLD 0.00005f

#define GAMMA 0.9f
#define ALPHA 0.1f
#define EPSILON_START 0.1f
#define EPSILON_DECAY 1.0f

//TYPE 0 => normal; 1 => 2 builders;
#define RL_TYPE 0

#define RL_FILE_DELETE true
#define RL_FILE_PATH ""

#define NUM_Q_TABLES 3
#define RL_FILE_ATTACK_MEX "qattackmex.dat"
#define RL_FILE_ATTACK_SOL "qattacksol.dat"
#define RL_FILE_ATTACK_WIN "qattackwin.dat"

#define RL_FILE_ATTACK_MEX_VISITS "qattackmex_visits.dat"
#define RL_FILE_ATTACK_SOL_VISITS "qattacksol_visits.dat"
#define RL_FILE_ATTACK_WIN_VISITS "qattackwin_visits.dat"

#define FILE_HEADER "QB"

#define RL_UPDATE_TIMEOUT 2400
using namespace std;

#include "types.h"
#include "RL_Action.h"
#include "RL_State.h"
#include "RL_Q.h"

namespace brainSpace {	

	struct DataPoint
	{
		RL_State prevState;
		RL_Action prevAction;
		RL_State resultState;
		float reward;
		float duration;
		float eligibilityTrace;

		DataPoint(RL_State ps, RL_Action pa, RL_State rs, float r, float d)
		{
			prevState = ps;
			prevAction = pa;
			resultState = rs;
			reward = r;
			duration = d;
			eligibilityTrace = 1;
		}
	};

	class RL
	{
	public:
		RL( AIClasses* aiClasses, double epsilon, int numAgents = 1);
		virtual ~RL();
		float GetTotalReward();
		char* GetFilePath(int type = 0);

		RL_Action* Update(MilitaryUnitGroup* group );

		void SetMayUpdate(bool mayUpdate );
		bool MayUpdate();

	private:
		AIClasses* ai;
		vector<DataPoint> dataTrail;
		RL_State* nullState;
		//RL_Action* nullAction;

		bool FileExists( const char* name );
		RL_Action* FindNextAction( RL_State *state, int type = 0 );
		RL_Action* FindBestAction( RL_State *state, int type = 0 );

		RL_State* PreviousState;
		RL_Action* PreviousAction;
		float PreviousFrame;
		float totalReward;
		bool goalAchieved;
		int Epsilon;
		RL_State* GetState(MilitaryUnitGroup* group, vector<pair<int, SAIFloat3> > resourceBuildings, int type = 0 );
		RL_Action* SafeNextAction(RL_State *state, int type = 0);

		void LoadFromFile(int type = 0 );
		void SaveToFile( int type = 0);

		bool m_greedyChoice;

		double EPSILON;

		//DefID, count
		map<int,int> StartGroup;
		vector<vector<QStateVar> > StateVars;
		vector<vector<QAction> > Actions;
		vector<RL_Q*> ValueFunction;
		int CurrentQTable;

		bool MayUpdateVar;
	};
}

#endif
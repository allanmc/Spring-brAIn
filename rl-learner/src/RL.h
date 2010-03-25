#ifndef _BRAINSPACE_RL_H
#define _BRAINSPACE_RL_H

#define RL_SOLAR_INDEX 15
#define RL_MEX_INDEX 2
#define RL_LAB_INDEX 10
#define RL_PLANT_INDEX 5
#define RL_ROCKO_INDEX 40

#define QBFILE_VERSION 2

#define COMMON_TERMINATION_REWARD true

//#define NUM_LEARNERS 3
extern int NUM_LEARNERS;
extern int RUNS;

#define TEST_RESULTS false

#define PRINT_REWARD true

#define USE_QSMDP true
#define USE_RS_TERMINATION true
#define USE_RS_TIME false
#define USE_Q_LAMBDA false
#define LAMBDA 0.95f
#define Q_LAMBDA_THRESHOLD 0.00005f

#define REWARD_METAL_MIN 2.5
#define REWARD_METAL_MAX 8.0
#define REWARD_ENERGY_MIN 15.0
#define REWARD_ENERGY_MAX 26.0

//#define GAMMA 0.9f
extern float GAMMA;
//#define ALPHA 0.1f
extern float ALPHA;
//#define EPSILON_START 0.5f
extern float EPSILON_START;
//#define EPSILON_DECAY 0.99999f
extern float EPSILON_DECAY;

//TYPE 0 => normal; 
#define RL_TYPE 0

#define RL_FILE_DELETE true
#define RL_FILE_PATH ""
#define RL_FILE_1 "qn.bin"
#define RL_FILE_2_BUILDERS "q2.bin"
#define RL_FILE_NO_END "qc.dat"
#define RL_FILE_3 "q3.dat"

#define RL_FILE_STATEVISITS "statevisits.dat"
#define RL_FILE_ACTIONSTATEVISITS "actionstatevisits.dat"

#define FILE_HEADER "QB"

#include "RL_State.h"
#include "RL_Q.h"
#include "game.h"
#include <vector>
#include <cmath>
#include "RL_Action.h"

using namespace std;

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
		RL(Game *g, double epsilon, int numAgents = 1, bool load = false);
		virtual ~RL();
		float GetTotalReward();
		static char* GetFilePath();
		unsigned int numActions;
		unsigned int numStates;
		int LastActionID;
		int LastStateID;

		RL_Action Update(int agentId = 0);
		void SaveToFile(bool doIt = false);
	private:
		Game *game;
		vector<DataPoint> dataTrail;
		RL_State nullState;
		RL_Action nullAction;
		bool *isTerminated;
		float lastTerminationReward;

		bool FileExists( const char* name );
		RL_Action FindNextAction( RL_State &state );
		RL_Action FindBestAction( RL_State &state );

		vector<RL_State> PreviousState;
		vector<RL_Action> PreviousAction;
		vector<double> PreviousFrame;
		float totalReward;
		bool goalAchieved;
		int Epsilon;
		RL_State GetState(int agentId = 0);
		RL_Action SafeNextAction(RL_State &state);
		
		void LoadFromFile(bool doIt = false);
		
		RL_Q* ValueFunction;

		bool m_greedyChoice;

		double EPSILON;
	};
}

#endif
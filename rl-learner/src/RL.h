#ifndef _BRAINSPACE_RL_H
#define _BRAINSPACE_RL_H

#define RL_SOLAR_INDEX 15
#define RL_MEX_INDEX 20
#define RL_LAB_INDEX 5
#define RL_PLANT_INDEX 5
#define RL_ROCKO_INDEX 40

#define QBFILE_VERSION 2

#define RL_NUM_NODES 1

#define PRINT_REWARD false

#define USE_QSMDP true
#define USE_RS_TIME true
#define USE_RS_LABS false
#define USE_BACKTRACKING false
#define USE_N_STEP false
#define USE_Q_LAMBDA false
#define LAMBDA 0.95
#define Q_LAMBDA_THRESHOLD 0.00005


//zero means infinite (Used both for backtracking and n-step)
#define BACKTRACKING_STEPS 0

#define GAMMA 0.9
#define ALPHA 0.1
#define EPSILON_START 0.1
#define EPSILON_DECAY 1

#define FILE_HEADER "QB"

#include "RL_State.h"
#include "RL_Q.h"
#include "game.h"
#include <vector>
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
		RL(Game *g, unsigned short int type, double epsilon, int numAgents = 1);
		virtual ~RL();
		float GetTotalReward();

		RL_Action Update(int agentId = 0);
	private:
		Game *game;
		vector<DataPoint> dataTrail;
		RL_State nullState;
		RL_Action nullAction;
		unsigned short int type;

		bool FileExists( const char* name );
		RL_Action FindNextAction( RL_State &state );
		RL_Action FindBestAction( RL_State &state );

		vector<RL_State> PreviousState;
		vector<RL_Action> PreviousAction;
		vector<float> PreviousFrame;
		float totalReward;
		bool goalAchieved;
		int Epsilon;
		RL_State GetState(int agentId = 0);
		RL_Action SafeNextAction(RL_State &state);
		void LoadFromFile();
		void SaveToFile();
		RL_Q* ValueFunction;

		bool m_greedyChoice;

		double EPSILON;
	};
}

#endif
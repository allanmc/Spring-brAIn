#ifndef _BRAINSPACE_RL_H
#define _BRAINSPACE_RL_H

#define RL_SOLAR_INDEX 15
#define RL_MEX_INDEX 20
#define RL_LAB_INDEX 2
#define RL_PLANT_INDEX 5
#define RL_ROCKO_INDEX 40

#define QBFILE_VERSION 2

#define PRINT_REWARD true

#define USE_QSMDP false
#define USE_RS_TERMINATION false
#define USE_RS_TIME true
#define USE_Q_LAMBDA false
#define LAMBDA 0.95f
#define Q_LAMBDA_THRESHOLD 0.00005f

#define GAMMA 0.9f
#define ALPHA 0.1f
#define EPSILON_START 1.0f
#define EPSILON_DECAY 0.9999f

//TYPE 0 => normal; 1 => 2 builders;
#define RL_TYPE 2

#define RL_FILE_DELETE true
#define RL_FILE_PATH ""
#define RL_FILE_1 "qn.bin"
#define RL_FILE_2_BUILDERS "q2.bin"
#define RL_FILE_NO_END "qc.dat"

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
		RL(Game *g, double epsilon, int numAgents = 1);
		virtual ~RL();
		float GetTotalReward();
		char* GetFilePath();

		RL_Action Update(int agentId = 0);
	private:
		Game *game;
		vector<DataPoint> dataTrail;
		RL_State nullState;
		RL_Action nullAction;

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
		
		void LoadFromFile();
		void SaveToFile();
		RL_Q* ValueFunction;

		bool m_greedyChoice;

		double EPSILON;
	};
}

#endif
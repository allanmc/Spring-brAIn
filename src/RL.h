#ifndef _BRAINSPACE_RL_H
#define _BRAINSPACE_RL_H

#define NEWSCENARIO false

#define RL_SOLAR_INDEX ( NEWSCENARIO ? 10 : 20 )
#define RL_MEX_INDEX ( NEWSCENARIO ? 15 : 20)
#define RL_LAB_INDEX ( NEWSCENARIO ? 4:  5 )
#define RL_PLANT_INDEX 5
#define RL_ROCKO_INDEX 20
#define RL_ATTACK_ACTION -2

#define QBFILE_VERSION 2

#define RL_NUM_NODES 1

#define PRINT_REWARD false

#define USE_QSMDP false
#define USE_RS_TIME true
#define USE_RS_LABS false
#define USE_BACKTRACKING false
#define USE_N_STEP false
#define USE_Q_LAMBDA true
#define LAMBDA 0.95
#define Q_LAMBDA_THRESHOLD 0.00005



//zero means infinite (Used both for backtracking and n-step)
#define BACKTRACKING_STEPS 5
#define EPSILON_START -1.0
#define GAMMA 0.9
#define ALPHA 0.1
#define EPSILON_DECAY 1

#define FILE_HEADER "QB"

#include "global.h"
#include "RL_State.h"
#include "RL_Q.h"

using namespace springai;
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
		RL(AIClasses* aiClasses, unsigned short int type, double epsilon);
		virtual ~RL();
		float GetTotalReward();

		void setDesireToBuild(int buildingId);
		RL_Action Update();
		bool ShouldIUpdate();
		vector<RL_Action> PreviousAction;
	private:
		AIClasses* ai;
		RL_State nullState;
		RL_Action nullAction;
		int buildingToBuild;
		unsigned short int type;

		bool FileExists( const char* name );
		RL_Action FindNextAction( RL_State &state );
		RL_Action FindBestAction( RL_State &state );

		int currentNode;
		vector<RL_State> PreviousState;
		vector<float> PreviousFrame;
		vector<DataPoint> dataTrail;
		vector<int> ParentNode;


		float totalReward;
		bool goalAchieved;
		int Epsilon;
		RL_State GetState(int node);
		RL_Action SafeNextAction(RL_State &state);
		void TakeAction(RL_Action &action);
		void ClearAllNodes();
		void LoadFromFile();
		void SaveToFile();
		vector<RL_Q*> ValueFunction;
		bool m_greedyChoice;

		double EPSILON;
	};
}

#endif
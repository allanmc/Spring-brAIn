#ifndef _BRAINSPACE_GAME_H
#define _BRAINSPACE_GAME_H

#define RL_SOLAR_ID 0
#define RL_MEX_ID 1
#define RL_LAB_ID 2
//#define RL_PLANT_ID 3
#define RL_ROCKO_ID 3
#define RL_MEX_PRODUCTION 3.0
#define RL_SOLAR_PRODUCTION 20.0
#define COMMANDER_SPEED 300.0
#define LAB_SPEED 100.0

#define DISCRETE_STATES 7
#define DISCRETE_STATES_STEP 300

#define RL_TYPE 0

#define RL_FILE_DELETE false
#define RL_FILE_PATH ""
#define RL_FILE_1 "qn.bin"
#define RL_FILE_2 "qh.bin"
#define RL_FILE_3 "qr.bin"

#include <algorithm>
#include <math.h>

namespace brainSpace {

	struct unitdef
	{
		int metalCost;
		int energyCost;
		int buildTime;
		unitdef( int metal, int energy, int time)
		{
			metalCost = metal;
			energyCost = energy;
			buildTime = time;
		}
		unitdef() {}
	};

	class Game
	{
	public:
		Game();
		int greedy[2];
		int units[4];
		float GetProduction(int resourceId);
		int CanBuild(int unitId, int amount);
		void ConstructUnit(int unitId);
		short unsigned int GetDiscreteResource(float realValue);
		float GetAvailableResources(int resourceId, float time);
		float BuildingCosts(int resourceId, int unitId);
		float GetBuildTime(int unitId);
		float GetUsage(int resourceId);
		float frame;
		void ResetGame();
		float resources[2];
	private:
		
		float GetTimeToDepletion(float current, float production);
		unitdef unitDefs[4];
	};
}



#endif
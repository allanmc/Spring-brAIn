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
#define BUILDER_SPEED 90.0
#define LAB_SPEED 100.0

#define DISCRETE_STATES 7
#define DISCRETE_STATES_STEP 300

#define RL_TYPE 0

#define RL_FILE_DELETE true
#define RL_FILE_PATH ""
#define RL_FILE_1 "qn.bin"
#define RL_FILE_2 "qh.bin"
#define RL_FILE_3 "qr.bin"

#include <algorithm>
#include <math.h>
#include <vector>

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

	struct unitBeingBuilt
	{
		int unitId;
		float remainingMetal;
		float remainingEnergy;
	};

	class Game
	{
	public:
		Game();
		int greedy[2];
		int units[4];
		float GetProduction(int resourceId);
		int CanBuild(int unitId);
		void ConstructUnit(int unitId);
		short unsigned int GetDiscreteResource(float realValue);
		float GetAvailableResources(int resourceId, float time);
		float BuildingCosts(int resourceId, int unitId);
		float GetBuildTime(int unitId, bool commander);
		float GetUsage(int resourceId);
		float frame;
		void ResetGame();
		float resources[2];
		void BuildUnit(int unitId);
		void Update(); 

	private:
		int GetBuildingWithShortestBuildtime();
		float GetTimeToDepletion(float current, float production);
		unitdef unitDefs[4];
		std::vector<unitBeingBuilt> buildList;
	};
}



#endif
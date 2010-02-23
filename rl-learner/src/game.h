#ifndef _BRAINSPACE_GAME_H
#define _BRAINSPACE_GAME_H

#define NUM_UNIT_DEFS 4
#define RL_SOLAR_ID 0
#define RL_MEX_ID 1
#define RL_LAB_ID 2
#define RL_ROCKO_ID 3
#define RL_MEX_PRODUCTION 3.0
#define RL_SOLAR_PRODUCTION 20.0
#define COMMANDER_SPEED 300.0
#define BUILDER_SPEED 90.0
#define LAB_SPEED 100.0

#define DISCRETE_STATES 7
#define DISCRETE_STATES_STEP 300

#include <algorithm>
#include <math.h>
#include <vector>

namespace brainSpace {

	struct unitdef
	{
		int metalCost;
		int energyCost;
		int buildTime;
		float production[2];
		unitdef( int metal, int energy, int time, float metalProduction = 0, float energyProduction = 0)
		{
			metalCost = metal;
			energyCost = energy;
			buildTime = time;
			production[RL_MEX_ID] = metalProduction;
			production[RL_SOLAR_ID] = energyProduction;
		}
		unitdef() {}
	};

	struct unitBeingBuilt
	{
		int unitId;
		int builder;
		float remainingMetal;
		float remainingEnergy;
	};

	class Game
	{
	public:
		Game();
		int units[NUM_UNIT_DEFS];
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
		void BuildUnit(int unitId, int agentId = 0);
		std::vector<int> Update();
		int UnitBeingBuildByBuilder(int builder);

	private:
		int GetBuildingWithShortestBuildtime();
		float GetTimeToDepletion(float current, float production);
		unitdef unitDefs[NUM_UNIT_DEFS];
		std::vector<unitBeingBuilt> buildList;
	};
}



#endif
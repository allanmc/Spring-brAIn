#ifndef _BRAINSPACE_GAME_H
#define _BRAINSPACE_GAME_H

#define NUM_UNIT_DEFS 4
#define SOLAR_ID 0
#define MEX_ID 1
#define LAB_ID 2
#define ROCKO_ID 3
#define COMMANDER_SPEED 300.0f
#define BUILDER_SPEED 90.0f
#define LAB_SPEED 100.0f

#define DISCRETE_STATES 7
#define DISCRETE_STATES_STEP 300

#include <algorithm>
#include <math.h>
#include <vector>

extern int g_currentGame;

namespace brainSpace {

	struct unitdef
	{
		int metalCost;
		int energyCost;
		int buildTime;
		double production[2];
		unitdef( int metal, int energy, int time, double metalProduction = 0, double energyProduction = 0)
		{
			metalCost = metal;
			energyCost = energy;
			buildTime = time;
			production[MEX_ID] = metalProduction;
			production[SOLAR_ID] = energyProduction;
		}
		unitdef() {}
	};

	struct unitBeingBuilt
	{
		int unitId;
		int builder;
		double remainingMetal;
		double remainingEnergy;
	};

	class Game
	{
	public:
		Game();
		int units[NUM_UNIT_DEFS];
		double GetProduction(int resourceId);
		int CanBuild(int unitId);
		void ConstructUnit(int unitId);
		short unsigned int GetDiscreteResource(double realValue);
		double GetAvailableResources(int resourceId, double time);
		double BuildingCosts(int resourceId, int unitId);
		double GetBuildTime(int unitId, bool commander);
		double GetUsage(int resourceId);
		double frame;
		void ResetGame();
		double resources[2];
		void BuildUnit(int unitId, int agentId = 0);
		std::vector<int> Update();
		int UnitBeingBuildByBuilder(int builder);

	private:
		int GetBuildingWithShortestBuildtime();
		double GetTimeToDepletion(double current, double production);
		unitdef unitDefs[NUM_UNIT_DEFS];
		std::vector<unitBeingBuilt> buildList;
	};
}



#endif
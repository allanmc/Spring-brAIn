#ifndef _BRAINSPACE_GAME_H
#define _BRAINSPACE_GAME_H

#define RL_SOLAR_ID 0
#define RL_MEX_ID 1
#define RL_LAB_ID 2
#define RL_PLANT_ID 3
#define RL_MEX_PRODUCTION 3.0
#define RL_SOLAR_PRODUCTION 20.0
#define COMMANDER_SPEED 300.0

#define ROCKO_BUILDTIME 18.9
#define ROCKO_M_USE 5.1
#define ROCKO_E_USE 49.9

#define DISCRETE_STATES 7
#define DISCRETE_STATES_STEP 300

#define RL_TYPE 2

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
		int buildings[4];
		float GetProduction(int resourceId);
		int CanBuild(int buildingID);
		void ConstructBuilding(int buildingId);
		short unsigned int GetDiscreteResource(float realValue);
		float AvailableResources(int resourceId, float time);
		float BuildingCosts(int resourceId, int buildingID);
		float BuildTime(int buildingID);
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
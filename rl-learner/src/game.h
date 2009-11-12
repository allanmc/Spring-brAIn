#ifndef _BRAINSPACE_GAME_H
#define _BRAINSPACE_GAME_H

#define RL_SOLAR_ID 0
#define RL_MEX_ID 1
#define RL_LAB_ID 2
#define RL_PLANT_ID 3
#define RL_MEX_PRODUCTION 3
#define RL_SOLAR_PRODUCTION 20
#define COMMANDER_SPEED 300.0
#include <algorithm>

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
		int buildings[4];
		float GetProduction(int resourceId);
		int CanBuild(int buildingID);
		void ConstructBuilding(int buildingId);
		float frame;
		void ResetGame();
	private:
		float resources[2];
		float GetTimeToDepletion(float current, float production);
		unitdef unitDefs[4];
	};
}



#endif
#include "game.h"
#include <iostream>
using namespace brainSpace;
using namespace std;

Game::Game()
{
	unitDefs[RL_MEX_ID] = unitdef(50,521,1800);
	unitDefs[RL_SOLAR_ID] = unitdef(145,0,2845);
	unitDefs[RL_PLANT_ID] = unitdef(743,1853,7192);
	unitDefs[RL_LAB_ID] = unitdef(605,1130,6760);
	ResetGame();
	greedy[0] = 0;
	greedy[1] = 0;
}

void Game::ResetGame()
{
	resources[RL_MEX_ID] = 1000;
	resources[RL_SOLAR_ID] = 1000;
	buildings[RL_MEX_ID] = 0;
	buildings[RL_SOLAR_ID] = 0;
	buildings[RL_PLANT_ID] = 0;
	buildings[RL_LAB_ID] = 0;
	frame = 0;
}

void Game::ConstructBuilding(int buildingId)
{
	buildings[buildingId]++;
	float timeToBuild = unitDefs[buildingId].buildTime/(float)COMMANDER_SPEED;
	float incomeMetal = - unitDefs[buildingId].metalCost/timeToBuild - GetUsage(RL_MEX_ID);
	float incomeEnergy = - unitDefs[buildingId].energyCost/timeToBuild - GetUsage(RL_SOLAR_ID);
	float productionMetal = incomeMetal + GetProduction(RL_MEX_ID);
	float productionEnergy = incomeEnergy + GetProduction(RL_SOLAR_ID);
	
	float metalTime = GetTimeToDepletion(resources[RL_MEX_ID], productionMetal);
	float energyTime = GetTimeToDepletion(resources[RL_SOLAR_ID], productionEnergy);

	float metalCost = unitDefs[buildingId].metalCost + GetUsage(RL_MEX_ID)*timeToBuild;
	float energyCost = unitDefs[buildingId].energyCost + GetUsage(RL_SOLAR_ID)*timeToBuild;

	int canBuildVal = CanBuild(buildingId);
	if(canBuildVal == 0)
	{
		resources[RL_MEX_ID] += timeToBuild*productionMetal;
		resources[RL_SOLAR_ID] += timeToBuild*productionEnergy;
		frame += timeToBuild;
		//cout << "timetobuild " << timeToBuild << "\n";
	}
	else if(canBuildVal == -1)
	{
		float remainingMetalUse = metalCost - resources[RL_MEX_ID] - metalTime*GetProduction(RL_MEX_ID);
		resources[RL_MEX_ID] = 0;
		resources[RL_SOLAR_ID] += metalTime*productionEnergy;
		float remainingEnergy = energyCost + incomeEnergy*metalTime;
		float remainingTime = remainingMetalUse/GetProduction(RL_MEX_ID);
		resources[RL_SOLAR_ID] += GetProduction(RL_SOLAR_ID)*remainingTime - remainingEnergy;
		frame += metalTime+remainingTime;
		//cout << "metalTime " << metalTime << "\n";
		//cout << "remainingTime " << remainingTime << "\n";
	}
	else if(canBuildVal == -2)
	{
		float remainingEnergyUse = energyCost - resources[RL_SOLAR_ID] - energyTime*GetProduction(RL_SOLAR_ID);
		resources[RL_MEX_ID] += energyTime*productionMetal;
		resources[RL_SOLAR_ID] = 0;
		float remainingMetal = metalCost + incomeMetal*energyTime;
		float remainingTime = remainingEnergyUse/GetProduction(RL_SOLAR_ID);
		resources[RL_MEX_ID] += GetProduction(RL_MEX_ID)*remainingTime - remainingMetal;
		frame += energyTime+remainingTime;
		/*cout << "remainingTime " << remainingTime << "\n";
		cout << "energyTime " << energyTime << "\n";*/
	}
	else //canBuildVal == -3
	{
		if(metalTime < energyTime)
		{
			float remainingMetalUse = metalCost - resources[RL_MEX_ID] - metalTime*GetProduction(RL_MEX_ID);
			resources[RL_MEX_ID] = 0;
			resources[RL_SOLAR_ID] += metalTime*productionEnergy;
			float remainingEnergy = energyCost + incomeEnergy*metalTime;
			float remainingTime = remainingMetalUse/GetProduction(RL_MEX_ID);
			float energyProduction = -remainingEnergy/remainingTime + GetProduction(RL_SOLAR_ID);
			float energyDepletion = GetTimeToDepletion(resources[RL_SOLAR_ID], energyProduction);
			if(energyDepletion > remainingTime || energyDepletion == -1)
			{
				resources[RL_SOLAR_ID] += GetProduction(RL_SOLAR_ID)*remainingTime - remainingEnergy;
				frame += metalTime + remainingTime;
				//cout << "metalTime " << metalTime << "\n";
				//cout << "remainingTime " << remainingTime << "\n";
			}
			else
			{
				float remainingEnergyUse = remainingEnergy - resources[RL_SOLAR_ID] - energyTime*GetProduction(RL_SOLAR_ID);
				resources[RL_SOLAR_ID] = 0;
				float remainingMetal = metalCost + incomeMetal*energyDepletion;
				float newRemainingTime = remainingEnergyUse/GetProduction(RL_SOLAR_ID);
				resources[RL_MEX_ID] += GetProduction(RL_MEX_ID)*newRemainingTime - remainingMetal;
				frame += metalTime + remainingTime + newRemainingTime;
				//cout << "metalTime " << metalTime << "\n";
				//cout << "remainingTime" << remainingTime << "\n";
				//cout << "newRemainingTime " << newRemainingTime << "\n";
			}
		}
		else
		{
			float remainingEnergyUse = energyCost - resources[RL_SOLAR_ID] - energyTime*GetProduction(RL_SOLAR_ID);
			resources[RL_SOLAR_ID] = 0;
			resources[RL_MEX_ID] += energyTime*productionMetal;
			float remainingMetal = metalCost + incomeMetal*energyTime;
			float remainingTime = remainingEnergyUse/GetProduction(RL_SOLAR_ID);
			float metalProduction = -remainingMetal/remainingTime + GetProduction(RL_MEX_ID);
			float metalDepletion = GetTimeToDepletion(resources[RL_MEX_ID], metalProduction);
			if(metalDepletion > remainingTime || metalDepletion == -1)
			{
				resources[RL_MEX_ID] += GetProduction(RL_MEX_ID)*remainingTime - remainingMetal;
				frame += energyTime + remainingTime;
				//cout << "energyTime " << energyTime << "\n";
				//cout << "remainingTime " << remainingTime << "\n";
			}
			else
			{
				float remainingMetalUse = remainingMetal - resources[RL_MEX_ID] - metalTime*GetProduction(RL_MEX_ID);
				resources[RL_MEX_ID] = 0;
				float remainingEnergy = energyCost + incomeEnergy*metalDepletion;
				float newRemainingTime = remainingMetalUse/GetProduction(RL_MEX_ID);
				resources[RL_SOLAR_ID] += GetProduction(RL_SOLAR_ID)*newRemainingTime - remainingEnergy;
				frame += energyTime + remainingTime + newRemainingTime;
				//cout << "energyTime " << energyTime << "\n";
				//cout << "remainingTime " << remainingTime << "\n";
				//cout << "newRemainingTime " << newRemainingTime << "\n";
			}
		}	
	}
	if(resources[RL_SOLAR_ID] > 1000)
		resources[RL_SOLAR_ID] = 1000;
	
	if(resources[RL_MEX_ID] > 1000)
		resources[RL_MEX_ID] = 1000;
}

short unsigned int Game::GetDiscreteResource(float realValue)
{
	for (short unsigned int i = 0; i < DISCRETE_STATES; i++)
	{
		if (realValue < DISCRETE_STATES_STEP*(i+1))
		{
			return i;
		}
	}
}

float Game::AvailableResources(int resourceId, float time)
{
	return resources[resourceId] + GetProduction(resourceId)*time;
}

float Game::BuildTime(int buildingID)
{
	return unitDefs[buildingID].buildTime/(float)COMMANDER_SPEED;
}

float Game::BuildingCosts(int resourceId, int buildingID)
{
	if(resourceId == RL_MEX_ID)
	{
		return unitDefs[buildingID].metalCost;
	}
	else
	{
		return unitDefs[buildingID].energyCost;
	}
}

int Game::CanBuild(int buildingID)
{
	int retVal = 0;
	float timeToBuild = unitDefs[buildingID].buildTime/(float)COMMANDER_SPEED;
	float incomeMetal = - unitDefs[buildingID].metalCost/timeToBuild;
	float incomeEnergy = - unitDefs[buildingID].energyCost/timeToBuild;
	float productionMetal = incomeMetal + GetProduction(RL_MEX_ID);
	float productionEnergy = incomeEnergy + GetProduction(RL_SOLAR_ID);
	
	float metalTime = GetTimeToDepletion(resources[RL_MEX_ID], productionMetal);
	float energyTime = GetTimeToDepletion(resources[RL_SOLAR_ID], productionEnergy);
	
	retVal += (metalTime>=0 && metalTime<timeToBuild ? -1 : 0);
	retVal += (energyTime>=0 && energyTime<timeToBuild ? -2 : 0);

	return retVal;
}

float Game::GetTimeToDepletion(float current, float production)
{
	if(production >= 0)
		return -1;
	else
	{
		return -current/production;
	}

}

float Game::GetUsage(int resourceId)
{
	 if (resourceId == RL_MEX_ID)
	 {
		return buildings[RL_LAB_ID]*ROCKO_M_USE;
	 }
	 else
	 {
		 return buildings[RL_LAB_ID]*ROCKO_E_USE;
	 }
}
float Game::GetProduction(int resourceId)
{
	float production = 0;
	if(resourceId == RL_MEX_ID)
	{
		production = (float)RL_MEX_PRODUCTION*buildings[resourceId];
		production += 1.5;
		//production -= buildings[RL_LAB_ID]*ROCKO_M_USE;//Simulate continuous Rocko production
	}
	else
	{
		production = (float)RL_SOLAR_PRODUCTION*buildings[resourceId];
		production += 25;
		//production -= buildings[RL_LAB_ID]*ROCKO_E_USE;//Simulate continuous Rocko production
	}	
	return production;
}
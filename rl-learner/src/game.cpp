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
	float incomeMetal = - unitDefs[buildingId].metalCost/timeToBuild;
	float incomeEnergy = - unitDefs[buildingId].energyCost/timeToBuild;
	float productionMetal = incomeMetal + GetProduction(RL_MEX_ID);
	float productionEnergy = incomeEnergy + GetProduction(RL_SOLAR_ID);
	
	float metalTime = GetTimeToDepletion(resources[RL_MEX_ID], productionMetal);
	float energyTime = GetTimeToDepletion(resources[RL_SOLAR_ID], productionEnergy);

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
		float remainingMetalUse = unitDefs[buildingId].metalCost - resources[RL_MEX_ID] - metalTime*GetProduction(RL_MEX_ID);
		resources[RL_MEX_ID] = 0;
		resources[RL_SOLAR_ID] += metalTime*productionEnergy;
		float remainingEnergy = unitDefs[buildingId].energyCost + incomeEnergy*metalTime;
		float remainingTime = remainingMetalUse/GetProduction(RL_MEX_ID);
		resources[RL_SOLAR_ID] += GetProduction(RL_SOLAR_ID)*remainingTime - remainingEnergy;
		frame += metalTime+remainingTime;
		//cout << "metalTime " << metalTime << "\n";
		//cout << "remainingTime " << remainingTime << "\n";
	}
	else if(canBuildVal == -2)
	{
		float remainingEnergyUse = unitDefs[buildingId].energyCost - resources[RL_SOLAR_ID] - energyTime*GetProduction(RL_SOLAR_ID);
		resources[RL_MEX_ID] += energyTime*productionMetal;
		resources[RL_SOLAR_ID] = 0;
		float remainingMetal = unitDefs[buildingId].metalCost + incomeMetal*energyTime;
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
			float remainingMetalUse = unitDefs[buildingId].metalCost - resources[RL_MEX_ID] - metalTime*GetProduction(RL_MEX_ID);
			resources[RL_MEX_ID] = 0;
			resources[RL_SOLAR_ID] += metalTime*productionEnergy;
			float remainingEnergy = unitDefs[buildingId].energyCost + incomeEnergy*metalTime;
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
				float remainingMetal = unitDefs[buildingId].metalCost + incomeMetal*energyDepletion;
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
			float remainingEnergyUse = unitDefs[buildingId].energyCost - resources[RL_SOLAR_ID] - energyTime*GetProduction(RL_SOLAR_ID);
			resources[RL_SOLAR_ID] = 0;
			resources[RL_MEX_ID] += energyTime*productionMetal;
			float remainingMetal = unitDefs[buildingId].metalCost + incomeMetal*energyTime;
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
				float remainingEnergy = unitDefs[buildingId].energyCost + incomeEnergy*metalDepletion;
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

float Game::GetProduction(int resourceId)
{
	float production = 0;
	if(resourceId == RL_MEX_ID)
	{
		production = (float)RL_MEX_PRODUCTION*buildings[resourceId];
		production += 1.5;
	}
	else
	{
		production = (float)RL_SOLAR_PRODUCTION*buildings[resourceId];
		production += 25;
	}	
	return production;
}
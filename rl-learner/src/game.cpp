#include "game.h"
#include <iostream>
using namespace brainSpace;
using namespace std;

Game::Game()
{
	unitDefs[RL_MEX_ID] = unitdef(50,521,1800);
	unitDefs[RL_SOLAR_ID] = unitdef(145,0,2845);
	//unitDefs[RL_PLANT_ID] = unitdef(743,1853,7192);
	unitDefs[RL_LAB_ID] = unitdef(605,1130,6760);
	unitDefs[RL_ROCKO_ID] = unitdef(97,944,1887);
	ResetGame();
	greedy[0] = 0;
	greedy[1] = 0;
}

void Game::ResetGame()
{
	resources[RL_MEX_ID] = 1000;
	resources[RL_SOLAR_ID] = 1000;
	units[RL_MEX_ID] = 0;
	units[RL_SOLAR_ID] = 0;
	units[RL_LAB_ID] = 0;
	units[RL_ROCKO_ID] = 0;
	frame = 0;
}

void Game::BuildUnit(int unitId, int agentId)
{
	unitBeingBuilt b;
	b.builder = agentId;
	b.unitId = unitId;
	b.remainingEnergy = unitDefs[unitId].energyCost;
	b.remainingMetal = unitDefs[unitId].metalCost;
	buildList.push_back(b);
}

vector<int> Game::Update()
{
	float metalUse = 0;
	float energyUse = 0;
	float metalProd = GetProduction(RL_MEX_ID);
	float energyProd = GetProduction(RL_SOLAR_ID);
	resources[RL_MEX_ID] += metalProd;
	resources[RL_SOLAR_ID] += energyProd;
	
	float TotalMetalCost = 0;
	float TotalEnergyCost = 0;
	
	for(int i = 0; i < buildList.size(); i++)
	{
		float time = GetBuildTime(buildList[i].unitId, false);
		energyUse += unitDefs[buildList[i].unitId].energyCost/time;
		metalUse += unitDefs[buildList[i].unitId].metalCost/time;
		TotalEnergyCost += unitDefs[buildList[i].unitId].energyCost;
		TotalMetalCost += unitDefs[buildList[i].unitId].metalCost;
	}

	if(metalUse < resources[RL_MEX_ID] && energyUse < resources[RL_SOLAR_ID])
	{
		//we wont run out this time
		resources[RL_SOLAR_ID] -= energyUse;
		resources[RL_MEX_ID] -= metalUse;
		for(int i = 0; i < buildList.size(); i++)
		{
			float time = GetBuildTime(buildList[i].unitId, false);
			buildList[i].remainingEnergy -= unitDefs[buildList[i].unitId].energyCost/time;
			buildList[i].remainingMetal -= unitDefs[buildList[i].unitId].metalCost/time;
		}
	}
	else if(metalUse < resources[RL_MEX_ID] && energyUse > resources[RL_SOLAR_ID])
	{
		//run out of energy
		float p = resources[RL_SOLAR_ID]/energyUse;
		for(int i = 0; i < buildList.size(); i++)
		{
			float time = GetBuildTime(buildList[i].unitId, false);
			buildList[i].remainingEnergy -= p*unitDefs[buildList[i].unitId].energyCost/time;
			buildList[i].remainingMetal -= p*unitDefs[buildList[i].unitId].metalCost/time;
		}
		resources[RL_SOLAR_ID] = 0;
		resources[RL_MEX_ID] -= p*metalUse;
	}
	else if(metalUse > resources[RL_MEX_ID] && energyUse < resources[RL_SOLAR_ID])
	{
		//run out of metal
		float p = resources[RL_MEX_ID]/metalUse;
		for(int i = 0; i < buildList.size(); i++)
		{
			float time = GetBuildTime(buildList[i].unitId, false);
			buildList[i].remainingEnergy -= p*unitDefs[buildList[i].unitId].energyCost/time;
			buildList[i].remainingMetal -= p*unitDefs[buildList[i].unitId].metalCost/time;
		}
		resources[RL_SOLAR_ID] -= p*energyUse;
		resources[RL_MEX_ID] = 0;
	}
	else
	{
		//run out of something.. find out what
		float pm = resources[RL_MEX_ID]/metalUse;
		float pe = resources[RL_SOLAR_ID]/energyUse;
		if(pm < pe)
		{
			for(int i = 0; i < buildList.size(); i++)
			{
				float time = GetBuildTime(buildList[i].unitId, false);
				buildList[i].remainingEnergy -= pm*unitDefs[buildList[i].unitId].energyCost/time;
				buildList[i].remainingMetal -= pm*unitDefs[buildList[i].unitId].metalCost/time;
			}
			resources[RL_SOLAR_ID] -= pm*energyUse;
			resources[RL_MEX_ID] = 0;
		}
		else 
		{
			for(int i = 0; i < buildList.size(); i++)
			{
				float time = GetBuildTime(buildList[i].unitId, false);
				buildList[i].remainingEnergy -= pe*unitDefs[buildList[i].unitId].energyCost/time;
				buildList[i].remainingMetal -= pe*unitDefs[buildList[i].unitId].metalCost/time;
			}
			resources[RL_SOLAR_ID] = 0;
			resources[RL_MEX_ID] -= pe*metalUse;
		}
	}
	if(resources[RL_MEX_ID] < 0 || resources[RL_SOLAR_ID] < 0 || metalUse < 0 || energyUse < 0)
	{
		bool hest = true;
	}

	//anyone finished? (TODO: What if more than one are finished at the same time?)
	vector<int> finished;
	for(int i = 0; i < buildList.size(); i++)
	{
		if( buildList[i].remainingEnergy <= 0)
		{
			resources[RL_MEX_ID] -= buildList[i].remainingMetal;
			resources[RL_SOLAR_ID] -= buildList[i].remainingEnergy;
			units[buildList[i].unitId]++;
			finished.push_back(buildList[i].builder);
			buildList.erase(buildList.begin() + i);
			i--;
			
		}
	}
	

	//if(resources[RL_SOLAR_ID] > 0 && resources[RL_MEX_ID] > 0)
	//{
	//	int id = GetBuildingWithShortestBuildtime();
	//	int ecost = unitDefs[buildList[id].unitId].energyCost;
	//	int buildTime = (buildList[id].remainingEnergy/ecost)*unitDefs[buildList[id].unitId].buildTime;
	//	if(timeToMetalDepletion > -1 && timeToMetalDepletion < buildTime/BUILDER_SPEED)
	//	{

	//	}
	//    if(timeToEnergyDepletion > -1 && timeToEnergyDepletion < buildTime/BUILDER_SPEED)
	//	{

	//	}
	//}

	
	if(resources[RL_SOLAR_ID] > 1000)
		resources[RL_SOLAR_ID] = 1000;
	
	if(resources[RL_MEX_ID] > 1000)
		resources[RL_MEX_ID] = 1000;
	return finished;

}

int Game::GetBuildingWithShortestBuildtime()
{
	int shortestIt = 0;
	int currentMin = 9999999999;
	for(int i = 0; i < buildList.size(); i++)
	{
		int id = buildList[i].unitId;
		int ecost = unitDefs[id].energyCost;
		int buildTime = (buildList[i].remainingEnergy/ecost)*unitDefs[id].buildTime;
		if(buildTime < currentMin)
		{
			shortestIt = i;
			currentMin = buildTime;
		}
	}
	return shortestIt;
}

void Game::ConstructUnit(int unitId)
{
	int unitsToBuild = 1;
	units[unitId] += unitsToBuild;

	float timeToBuild = GetBuildTime(unitId, false);
	float metalCostPerSecond = - (unitDefs[unitId].metalCost*unitsToBuild)/timeToBuild - GetUsage(RL_MEX_ID);
	float energyCostPerSecond = - (unitDefs[unitId].energyCost*unitsToBuild)/timeToBuild - GetUsage(RL_SOLAR_ID);
	float metalIncome = metalCostPerSecond + GetProduction(RL_MEX_ID);
	float energyIncome = energyCostPerSecond + GetProduction(RL_SOLAR_ID);
	
	float timeToMetalDepletion = GetTimeToDepletion(resources[RL_MEX_ID], metalIncome);
	float timeToEnergyDepletion = GetTimeToDepletion(resources[RL_SOLAR_ID], energyIncome);

	float metalCost = unitDefs[unitId].metalCost + GetUsage(RL_MEX_ID)*timeToBuild;
	float energyCost = unitDefs[unitId].energyCost + GetUsage(RL_SOLAR_ID)*timeToBuild;

	int canBuildVal = CanBuild(unitId);
	if(canBuildVal == 0)
	{
		resources[RL_MEX_ID] += timeToBuild*metalIncome;
		resources[RL_SOLAR_ID] += timeToBuild*energyIncome;
		frame += timeToBuild;
		//cout << "timetobuild " << timeToBuild << "\n";
	}
	else if(canBuildVal == -1)
	{
		float remainingMetalToBeUsed = metalCost - resources[RL_MEX_ID] - timeToMetalDepletion*GetProduction(RL_MEX_ID);
		resources[RL_MEX_ID] = 0;
		resources[RL_SOLAR_ID] += timeToMetalDepletion*energyIncome;
		float remainingEnergy = energyCost + energyCostPerSecond*timeToMetalDepletion;
		float remainingTime = remainingMetalToBeUsed/GetProduction(RL_MEX_ID);
		resources[RL_SOLAR_ID] += GetProduction(RL_SOLAR_ID)*remainingTime - remainingEnergy;
		frame += timeToMetalDepletion+remainingTime;
		//cout << "metalTime " << metalTime << "\n";
		//cout << "remainingTime " << remainingTime << "\n";
	}
	else if(canBuildVal == -2)
	{
		float remainingEnergyToBeUsed = energyCost - resources[RL_SOLAR_ID] - timeToEnergyDepletion*GetProduction(RL_SOLAR_ID);
		resources[RL_MEX_ID] += timeToEnergyDepletion*metalIncome;
		resources[RL_SOLAR_ID] = 0;
		float remainingMetal = metalCost + metalCostPerSecond*timeToEnergyDepletion;
		float remainingTime = remainingEnergyToBeUsed/GetProduction(RL_SOLAR_ID);
		resources[RL_MEX_ID] += GetProduction(RL_MEX_ID)*remainingTime - remainingMetal;
		frame += timeToEnergyDepletion+remainingTime;
		/*cout << "remainingTime " << remainingTime << "\n";
		cout << "energyTime " << energyTime << "\n";*/
	}
	else //canBuildVal == -3
	{
		if(timeToMetalDepletion < timeToEnergyDepletion)
		{
			float remainingMetalUse = metalCost - resources[RL_MEX_ID] - timeToMetalDepletion*GetProduction(RL_MEX_ID);
			resources[RL_MEX_ID] = 0;
			resources[RL_SOLAR_ID] += timeToMetalDepletion*energyIncome;
			float remainingEnergy = energyCost + energyCostPerSecond*timeToMetalDepletion;
			float remainingTime = remainingMetalUse/GetProduction(RL_MEX_ID);
			float energyProduction = -remainingEnergy/remainingTime + GetProduction(RL_SOLAR_ID);
			float energyDepletion = GetTimeToDepletion(resources[RL_SOLAR_ID], energyProduction);
			if(energyDepletion > remainingTime || energyDepletion == -1)
			{
				resources[RL_SOLAR_ID] += GetProduction(RL_SOLAR_ID)*remainingTime - remainingEnergy;
				frame += timeToMetalDepletion + remainingTime;
				//cout << "metalTime " << metalTime << "\n";
				//cout << "remainingTime " << remainingTime << "\n";
			}
			else
			{
				float remainingEnergyUse = remainingEnergy - resources[RL_SOLAR_ID] - timeToEnergyDepletion*GetProduction(RL_SOLAR_ID);
				resources[RL_SOLAR_ID] = 0;
				float remainingMetal = metalCost + metalCostPerSecond*energyDepletion;
				float newRemainingTime = remainingEnergyUse/GetProduction(RL_SOLAR_ID);
				resources[RL_MEX_ID] += GetProduction(RL_MEX_ID)*newRemainingTime - remainingMetal;
				frame += timeToMetalDepletion + remainingTime + newRemainingTime;
				//cout << "metalTime " << metalTime << "\n";
				//cout << "remainingTime" << remainingTime << "\n";
				//cout << "newRemainingTime " << newRemainingTime << "\n";
			}
		}
		else
		{
			float remainingEnergyUse = energyCost - resources[RL_SOLAR_ID] - timeToEnergyDepletion*GetProduction(RL_SOLAR_ID);
			resources[RL_SOLAR_ID] = 0;
			resources[RL_MEX_ID] += timeToEnergyDepletion*metalIncome;
			float remainingMetal = metalCost + metalCostPerSecond*timeToEnergyDepletion;
			float remainingTime = remainingEnergyUse/GetProduction(RL_SOLAR_ID);
			float metalProduction = -remainingMetal/remainingTime + GetProduction(RL_MEX_ID);
			float metalDepletion = GetTimeToDepletion(resources[RL_MEX_ID], metalProduction);
			if(metalDepletion > remainingTime || metalDepletion == -1)
			{
				resources[RL_MEX_ID] += GetProduction(RL_MEX_ID)*remainingTime - remainingMetal;
				frame += timeToEnergyDepletion + remainingTime;
				//cout << "energyTime " << energyTime << "\n";
				//cout << "remainingTime " << remainingTime << "\n";
			}
			else
			{
				float remainingMetalUse = remainingMetal - resources[RL_MEX_ID] - timeToMetalDepletion*GetProduction(RL_MEX_ID);
				resources[RL_MEX_ID] = 0;
				float remainingEnergy = energyCost + energyCostPerSecond*metalDepletion;
				float newRemainingTime = remainingMetalUse/GetProduction(RL_MEX_ID);
				resources[RL_SOLAR_ID] += GetProduction(RL_SOLAR_ID)*newRemainingTime - remainingEnergy;
				frame += timeToEnergyDepletion + remainingTime + newRemainingTime;
				//cout << "energyTime " << energyTime << "\n";
				//cout << "remainingTime " << remainingTime << "\n";
				//cout << "newRemainingTime " << newRemainingTime << "\n";
			}
		}	
	}
}

short unsigned int Game::GetDiscreteResource(float realValue)
{
	short unsigned int i;
	for (i = 0; i < DISCRETE_STATES; i++)
	{
		if (realValue < DISCRETE_STATES_STEP*(i+1))
		{
			return i;
		}
	}
	return i;
}

float Game::GetAvailableResources(int resourceId, float time)
{
	return resources[resourceId] + GetProduction(resourceId)*time;
}

float Game::GetBuildTime(int unitId, bool commander)
{
	if (unitId == RL_ROCKO_ID)
	{
		return unitDefs[unitId].buildTime/(float)LAB_SPEED;
	} 
	else if(commander)
	{
		return unitDefs[unitId].buildTime/(float)COMMANDER_SPEED;
	}
	else
	{
		return unitDefs[unitId].buildTime/(float)BUILDER_SPEED;
	}
	
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

int Game::CanBuild(int unitId )
{
	int retVal = 0;
	float timeToBuild = GetBuildTime(unitId, false);

	float incomeMetal = - (unitDefs[unitId].metalCost)/timeToBuild - GetUsage(RL_MEX_ID);
	float incomeEnergy = - (unitDefs[unitId].energyCost)/timeToBuild - GetUsage(RL_SOLAR_ID);

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
	return 0;
	 if (resourceId == RL_MEX_ID)
	 {
		return units[RL_LAB_ID]*unitDefs[RL_ROCKO_ID].metalCost/LAB_SPEED;
	 }
	 else
	 {
		 return units[RL_LAB_ID]*unitDefs[RL_ROCKO_ID].energyCost/LAB_SPEED;
	 }
}
float Game::GetProduction(int resourceId)
{
	float production = 0;
	if(resourceId == RL_MEX_ID)
	{
		production = (float)RL_MEX_PRODUCTION*units[resourceId];
		production += 1.5;
		//production -= buildings[RL_LAB_ID]*ROCKO_M_USE;//Simulate continuous Rocko production
	}
	else
	{
		production = (float)RL_SOLAR_PRODUCTION*units[resourceId];
		production += 25;
		//production -= buildings[RL_LAB_ID]*ROCKO_E_USE;//Simulate continuous Rocko production
	}	
	return production;
}
#include "game.h"
#include <iostream>
using namespace brainSpace;
using namespace std;

int g_currentGame = 0;
Game::Game()
{
	unitDefs[MEX_ID] = unitdef(50,521,1800,3.0f,-3.0f);
	unitDefs[SOLAR_ID] = unitdef(145,0,2845,0.0f,20.0f);
	unitDefs[LAB_ID] = unitdef(605,1130,6760);
	unitDefs[ROCKO_ID] = unitdef(97,944,1887);
	ResetGame();
}

void Game::ResetGame()
{
	resources[MEX_ID] = 1000;
	resources[SOLAR_ID] = 1000;
	for(int i = 0; i<NUM_UNIT_DEFS; i++)
	{
		units[i] = 0;
	}
	buildList.clear();
	frame = 0;
}

void Game::BuildUnit(int unitId, int agentId)
{
	unitBeingBuilt b;
	b.builder = agentId;
	b.unitId = unitId;
	b.remainingEnergy = (float)unitDefs[unitId].energyCost;
	b.remainingMetal = (float)unitDefs[unitId].metalCost;
	buildList.push_back(b);
}

vector<int> Game::Update()
{
	float metalUse = 0;
	float energyUse = 0;
	float metalProd = GetProduction(MEX_ID);
	float energyProd = GetProduction(SOLAR_ID);
	if ( resources[MEX_ID] < 0 || resources[SOLAR_ID] < 0 )
	{
		cerr << "Metal: " << resources[MEX_ID] << endl;
		cerr << "Solar: " << resources[SOLAR_ID] << endl;
	}
	resources[MEX_ID] += metalProd;
	resources[SOLAR_ID] += energyProd;
	
	float TotalMetalCost = 0;
	float TotalEnergyCost = 0;


	for(unsigned int i = 0; i < buildList.size(); i++)
	{
		float time = GetBuildTime(buildList[i].unitId, false);
		energyUse += unitDefs[buildList[i].unitId].energyCost/time;
		metalUse += unitDefs[buildList[i].unitId].metalCost/time;
		TotalEnergyCost += unitDefs[buildList[i].unitId].energyCost;
		TotalMetalCost += unitDefs[buildList[i].unitId].metalCost;
	}

	if(metalUse < resources[MEX_ID] && energyUse < resources[SOLAR_ID])
	{
		//we wont run out this time
		resources[SOLAR_ID] -= energyUse;
		resources[MEX_ID] -= metalUse;
		if ( resources[MEX_ID] < 0 )
		{
			cerr << "0:MRes: " << resources[MEX_ID] << " remaining: " << buildList[0].remainingMetal << endl;
			getchar();
		}
		for(unsigned int i = 0; i < buildList.size(); i++)
		{
			float time = GetBuildTime(buildList[i].unitId, false);
			buildList[i].remainingEnergy -= unitDefs[buildList[i].unitId].energyCost/time;
			buildList[i].remainingMetal -= unitDefs[buildList[i].unitId].metalCost/time;
		}
	}
	else if(metalUse < resources[MEX_ID] && energyUse > resources[SOLAR_ID])
	{
		//run out of energy
		float p = resources[SOLAR_ID]/energyUse;
		for(unsigned int i = 0; i < buildList.size(); i++)
		{
			float time = GetBuildTime(buildList[i].unitId, false);
			buildList[i].remainingEnergy -= p*unitDefs[buildList[i].unitId].energyCost/time;
			buildList[i].remainingMetal -= p*unitDefs[buildList[i].unitId].metalCost/time;
		}
		resources[SOLAR_ID] = 0;
		resources[MEX_ID] -= p*metalUse;
			if ( resources[MEX_ID] < 0 )
			{
				cerr << "1:MRes: " << resources[MEX_ID] << " remaining: " << p*metalUse << endl;
				getchar();
			}
	}
	else if(metalUse > resources[MEX_ID] && energyUse < resources[SOLAR_ID])
	{
		//run out of metal
		float p = resources[MEX_ID]/metalUse;
		for(unsigned int i = 0; i < buildList.size(); i++)
		{
			float time = GetBuildTime(buildList[i].unitId, false);
			buildList[i].remainingEnergy -= p*unitDefs[buildList[i].unitId].energyCost/time;
			buildList[i].remainingMetal -= p*unitDefs[buildList[i].unitId].metalCost/time;
		}
		resources[SOLAR_ID] -= p*energyUse;
		resources[MEX_ID] = 0;

	}
	else
	{
		//run out of something.. find out what
		float pm = resources[MEX_ID]/metalUse;
		float pe = resources[SOLAR_ID]/energyUse;

		if(pm < pe)
		{
			for(unsigned int i = 0; i < buildList.size(); i++)
			{
				float time = GetBuildTime(buildList[i].unitId, false);
				buildList[i].remainingEnergy -= pm*unitDefs[buildList[i].unitId].energyCost/time;
				buildList[i].remainingMetal -= pm*unitDefs[buildList[i].unitId].metalCost/time;
			}
			resources[SOLAR_ID] -= pm*energyUse;
			resources[MEX_ID] = 0;
		}
		else 
		{
			for(unsigned int i = 0; i < buildList.size(); i++)
			{
				float time = GetBuildTime(buildList[i].unitId, false);
				buildList[i].remainingEnergy -= pe*unitDefs[buildList[i].unitId].energyCost/time;
				buildList[i].remainingMetal -= pe*unitDefs[buildList[i].unitId].metalCost/time;
			}
			resources[SOLAR_ID] = 0;
			resources[MEX_ID] -= pe*metalUse;
		}
	}

	if(resources[MEX_ID] < 0 || resources[SOLAR_ID] < 0 || metalUse < 0 || energyUse < 0)
	{
		bool hest = true;
	}

	//anyone finished?
	vector<int> finished;
	if ( buildList.size() == 0 )
	{
		cerr << buildList.size() << endl;
		getchar();
	}
	for(unsigned int i = 0; i < buildList.size(); i++)
	{

		if( buildList[i].remainingEnergy <= 0)
		{
			resources[MEX_ID] -= buildList[i].remainingMetal;
			if ( resources[MEX_ID] < 0 )
			{
				cerr << "MRes: " << resources[MEX_ID] << " remaining: " << buildList[i].remainingMetal << endl;
				getchar();
			}
			resources[SOLAR_ID] -= buildList[i].remainingEnergy;
			if ( buildList[i].unitId == LAB_ID )
			{
				//cerr << "Remaining metal: " << resources[MEX_ID] << " Remaining energy: " << resources[SOLAR_ID] << endl;
			}
			units[buildList[i].unitId]++;
			finished.push_back(buildList[i].builder);
			buildList.erase(buildList.begin() + i);
			i--;
			
		}
	}
	
	if(resources[SOLAR_ID] > 1000)
		resources[SOLAR_ID] = 1000;
	
	if(resources[MEX_ID] > 1000)
		resources[MEX_ID] = 1000;
	return finished;

}

int Game::GetBuildingWithShortestBuildtime()
{
	int shortestIt = 0;
	float currentMin = 99999999.0f;
	for(unsigned int i = 0; i < buildList.size(); i++)
	{
		int id = buildList[i].unitId;
		int ecost = unitDefs[id].energyCost;
		float buildTime = (buildList[i].remainingEnergy/ecost)*unitDefs[id].buildTime;
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
	float metalCostPerSecond = - (unitDefs[unitId].metalCost*unitsToBuild)/timeToBuild - GetUsage(MEX_ID);
	float energyCostPerSecond = - (unitDefs[unitId].energyCost*unitsToBuild)/timeToBuild - GetUsage(SOLAR_ID);
	float metalIncome = metalCostPerSecond + GetProduction(MEX_ID);
	float energyIncome = energyCostPerSecond + GetProduction(SOLAR_ID);
	
	float timeToMetalDepletion = GetTimeToDepletion(resources[MEX_ID], metalIncome);
	float timeToEnergyDepletion = GetTimeToDepletion(resources[SOLAR_ID], energyIncome);

	float metalCost = unitDefs[unitId].metalCost + GetUsage(MEX_ID)*timeToBuild;
	float energyCost = unitDefs[unitId].energyCost + GetUsage(SOLAR_ID)*timeToBuild;

	int canBuildVal = CanBuild(unitId);
	if(canBuildVal == 0)
	{
		resources[MEX_ID] += timeToBuild*metalIncome;
		resources[SOLAR_ID] += timeToBuild*energyIncome;
		frame += timeToBuild;
	}
	else if(canBuildVal == -1)
	{
		float remainingMetalToBeUsed = metalCost - resources[MEX_ID] - timeToMetalDepletion*GetProduction(MEX_ID);
		resources[MEX_ID] = 0;
		resources[SOLAR_ID] += timeToMetalDepletion*energyIncome;
		float remainingEnergy = energyCost + energyCostPerSecond*timeToMetalDepletion;
		float remainingTime = remainingMetalToBeUsed/GetProduction(MEX_ID);
		resources[SOLAR_ID] += GetProduction(SOLAR_ID)*remainingTime - remainingEnergy;
		frame += timeToMetalDepletion+remainingTime;
	}
	else if(canBuildVal == -2)
	{
		float remainingEnergyToBeUsed = energyCost - resources[SOLAR_ID] - timeToEnergyDepletion*GetProduction(SOLAR_ID);
		resources[MEX_ID] += timeToEnergyDepletion*metalIncome;
		resources[SOLAR_ID] = 0;
		float remainingMetal = metalCost + metalCostPerSecond*timeToEnergyDepletion;
		float remainingTime = remainingEnergyToBeUsed/GetProduction(SOLAR_ID);
		resources[MEX_ID] += GetProduction(MEX_ID)*remainingTime - remainingMetal;
		frame += timeToEnergyDepletion+remainingTime;
	}
	else //canBuildVal == -3
	{
		if(timeToMetalDepletion < timeToEnergyDepletion)
		{
			float remainingMetalUse = metalCost - resources[MEX_ID] - timeToMetalDepletion*GetProduction(MEX_ID);
			resources[MEX_ID] = 0;
			resources[SOLAR_ID] += timeToMetalDepletion*energyIncome;
			float remainingEnergy = energyCost + energyCostPerSecond*timeToMetalDepletion;
			float remainingTime = remainingMetalUse/GetProduction(MEX_ID);
			float energyProduction = -remainingEnergy/remainingTime + GetProduction(SOLAR_ID);
			float energyDepletion = GetTimeToDepletion(resources[SOLAR_ID], energyProduction);
			if(energyDepletion > remainingTime || energyDepletion == -1)
			{
				resources[SOLAR_ID] += GetProduction(SOLAR_ID)*remainingTime - remainingEnergy;
				frame += timeToMetalDepletion + remainingTime;
			}
			else
			{
				float remainingEnergyUse = remainingEnergy - resources[SOLAR_ID] - timeToEnergyDepletion*GetProduction(SOLAR_ID);
				resources[SOLAR_ID] = 0;
				float remainingMetal = metalCost + metalCostPerSecond*energyDepletion;
				float newRemainingTime = remainingEnergyUse/GetProduction(SOLAR_ID);
				resources[MEX_ID] += GetProduction(MEX_ID)*newRemainingTime - remainingMetal;
				frame += timeToMetalDepletion + remainingTime + newRemainingTime;
			}
		}
		else
		{
			float remainingEnergyUse = energyCost - resources[SOLAR_ID] - timeToEnergyDepletion*GetProduction(SOLAR_ID);
			resources[SOLAR_ID] = 0;
			resources[MEX_ID] += timeToEnergyDepletion*metalIncome;
			float remainingMetal = metalCost + metalCostPerSecond*timeToEnergyDepletion;
			float remainingTime = remainingEnergyUse/GetProduction(SOLAR_ID);
			float metalProduction = -remainingMetal/remainingTime + GetProduction(MEX_ID);
			float metalDepletion = GetTimeToDepletion(resources[MEX_ID], metalProduction);
			if(metalDepletion > remainingTime || metalDepletion == -1)
			{
				resources[MEX_ID] += GetProduction(MEX_ID)*remainingTime - remainingMetal;
				frame += timeToEnergyDepletion + remainingTime;
			}
			else
			{
				float remainingMetalUse = remainingMetal - resources[MEX_ID] - timeToMetalDepletion*GetProduction(MEX_ID);
				resources[MEX_ID] = 0;
				float remainingEnergy = energyCost + energyCostPerSecond*metalDepletion;
				float newRemainingTime = remainingMetalUse/GetProduction(MEX_ID);
				resources[SOLAR_ID] += GetProduction(SOLAR_ID)*newRemainingTime - remainingEnergy;
				frame += timeToEnergyDepletion + remainingTime + newRemainingTime;
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
	if (unitId == ROCKO_ID)
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
	if(resourceId == MEX_ID)
	{
		return (float)unitDefs[buildingID].metalCost;
	}
	else
	{
		return (float)unitDefs[buildingID].energyCost;
	}
}

int Game::CanBuild(int unitId )
{
	int retVal = 0;
	float timeToBuild = GetBuildTime(unitId, false);

	float incomeMetal = - (unitDefs[unitId].metalCost)/timeToBuild - GetUsage(MEX_ID);
	float incomeEnergy = - (unitDefs[unitId].energyCost)/timeToBuild - GetUsage(SOLAR_ID);

	float productionMetal = incomeMetal + GetProduction(MEX_ID);
	float productionEnergy = incomeEnergy + GetProduction(SOLAR_ID);
	
	float metalTime = GetTimeToDepletion(resources[MEX_ID], productionMetal);
	float energyTime = GetTimeToDepletion(resources[SOLAR_ID], productionEnergy);

	retVal += (metalTime>=0 && metalTime<timeToBuild ? -1 : 0);
	retVal += (energyTime>=0 && energyTime<timeToBuild ? -2 : 0);

	return retVal;
}

float Game::GetTimeToDepletion(float current, float production)
{
	if(production >= 0)
	{
		return -1;
	}
	else
	{
		return -current/production;
	}
}

float Game::GetUsage(int resourceId)
{
	return 0;
	if (resourceId == MEX_ID)
	{
		return units[LAB_ID]*unitDefs[ROCKO_ID].metalCost/LAB_SPEED;
	}
	else
	{
		return units[LAB_ID]*unitDefs[ROCKO_ID].energyCost/LAB_SPEED;
	}
}
float Game::GetProduction(int resourceId)
{
	float production = 0;
	for(int i=0; i<NUM_UNIT_DEFS; i++)
	{
		production += units[i]*unitDefs[i].production[resourceId];
	}
	if(resourceId == MEX_ID)
	{
		production += 1.5;
	}
	else
	{
		production += 25;
	}	
	return production;
}

int Game::UnitBeingBuildByBuilder(int builder)
{
	int UnitId = -1;
	for(unsigned int i = 0; i<buildList.size(); i++)
	{
		if(buildList[i].builder == builder)
		{
			UnitId = buildList[i].unitId;
			break;
		}
	}
	return UnitId;
}


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
	b.remainingEnergy = (double)unitDefs[unitId].energyCost;
	b.remainingMetal = (double)unitDefs[unitId].metalCost;
	buildList.push_back(b);
}

vector<int> Game::Update()
{
	double metalUse = 0;
	double energyUse = 0;
	double metalProd = GetProduction(MEX_ID);
	double energyProd = GetProduction(SOLAR_ID);
	if(resources[MEX_ID] < 0 || resources[SOLAR_ID] < 0 || metalUse < 0 || energyUse < 0)
	{
		cerr << "hest fail, m: " << resources[MEX_ID] << " e: " <<resources[SOLAR_ID] << " mu: " << metalUse << " eu: " << energyUse << "\n";
	}
	resources[MEX_ID] += metalProd;
	resources[SOLAR_ID] += energyProd;
	
	double TotalMetalCost = 0;
	double TotalEnergyCost = 0;
	
	for(unsigned int i = 0; i < buildList.size(); i++)
	{
		double time = GetBuildTime(buildList[i].unitId, false);
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
		for(unsigned int i = 0; i < buildList.size(); i++)
		{
			double time = GetBuildTime(buildList[i].unitId, false);
			buildList[i].remainingEnergy -= unitDefs[buildList[i].unitId].energyCost/time;
			buildList[i].remainingMetal -= unitDefs[buildList[i].unitId].metalCost/time;
		}
	}
	else if(metalUse < resources[MEX_ID] && energyUse > resources[SOLAR_ID])
	{
		//run out of energy
		double p = resources[SOLAR_ID]/energyUse;
		for(unsigned int i = 0; i < buildList.size(); i++)
		{
			double time = GetBuildTime(buildList[i].unitId, false);
			buildList[i].remainingEnergy -= p*unitDefs[buildList[i].unitId].energyCost/time;
			buildList[i].remainingMetal -= p*unitDefs[buildList[i].unitId].metalCost/time;
		}
		resources[SOLAR_ID] = 0;
		resources[MEX_ID] -= p*metalUse;
	}
	else if(metalUse > resources[MEX_ID] && energyUse < resources[SOLAR_ID])
	{
		//run out of metal
		double p = resources[MEX_ID]/metalUse;
		for(unsigned int i = 0; i < buildList.size(); i++)
		{
			double time = GetBuildTime(buildList[i].unitId, false);
			buildList[i].remainingEnergy -= p*unitDefs[buildList[i].unitId].energyCost/time;
			buildList[i].remainingMetal -= p*unitDefs[buildList[i].unitId].metalCost/time;
			if(buildList[i].remainingMetal <= 0 && buildList[i].remainingEnergy > 0 || buildList[i].remainingEnergy <= 0 && buildList[i].remainingMetal > 0)
			{
				bool hest = true;
			} 
		}
		resources[SOLAR_ID] -= p*energyUse;
		resources[MEX_ID] = 0;
	}
	else
	{
		//run out of something.. find out what
		double pm = resources[MEX_ID]/metalUse;
		double pe = resources[SOLAR_ID]/energyUse;
		if(pm < pe)
		{
			for(unsigned int i = 0; i < buildList.size(); i++)
			{
				double time = GetBuildTime(buildList[i].unitId, false);
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
				double time = GetBuildTime(buildList[i].unitId, false);
				buildList[i].remainingEnergy -= pe*unitDefs[buildList[i].unitId].energyCost/time;
				buildList[i].remainingMetal -= pe*unitDefs[buildList[i].unitId].metalCost/time;
			}
			resources[SOLAR_ID] = 0;
			resources[MEX_ID] -= pe*metalUse;
		}
	}


	//anyone finished?
	vector<int> finished;
	for(unsigned int i = 0; i < buildList.size(); i++)
	{
		if( buildList[i].remainingEnergy <= 0 && buildList[i].remainingMetal <= 0)
		{
			resources[MEX_ID] -= buildList[i].remainingMetal;
			resources[SOLAR_ID] -= buildList[i].remainingEnergy;
			if(resources[MEX_ID] < 0 || resources[SOLAR_ID] < 0)
				bool hest = true;
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

	if(resources[MEX_ID] < 0)
	{
		//resources[MEX_ID] = 0;
	}
	return finished;

}

int Game::GetBuildingWithShortestBuildtime()
{
	int shortestIt = 0;
	double currentMin = 99999999.0f;
	for(unsigned int i = 0; i < buildList.size(); i++)
	{
		int id = buildList[i].unitId;
		int ecost = unitDefs[id].energyCost;
		double buildTime = (buildList[i].remainingEnergy/ecost)*unitDefs[id].buildTime;
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

	double timeToBuild = GetBuildTime(unitId, false);
	double metalCostPerSecond = - (unitDefs[unitId].metalCost*unitsToBuild)/timeToBuild - GetUsage(MEX_ID);
	double energyCostPerSecond = - (unitDefs[unitId].energyCost*unitsToBuild)/timeToBuild - GetUsage(SOLAR_ID);
	double metalIncome = metalCostPerSecond + GetProduction(MEX_ID);
	double energyIncome = energyCostPerSecond + GetProduction(SOLAR_ID);
	
	double timeToMetalDepletion = GetTimeToDepletion(resources[MEX_ID], metalIncome);
	double timeToEnergyDepletion = GetTimeToDepletion(resources[SOLAR_ID], energyIncome);

	double metalCost = unitDefs[unitId].metalCost + GetUsage(MEX_ID)*timeToBuild;
	double energyCost = unitDefs[unitId].energyCost + GetUsage(SOLAR_ID)*timeToBuild;

	int canBuildVal = CanBuild(unitId);
	if(canBuildVal == 0)
	{
		resources[MEX_ID] += timeToBuild*metalIncome;
		resources[SOLAR_ID] += timeToBuild*energyIncome;
		frame += timeToBuild;
	}
	else if(canBuildVal == -1)
	{
		double remainingMetalToBeUsed = metalCost - resources[MEX_ID] - timeToMetalDepletion*GetProduction(MEX_ID);
		resources[MEX_ID] = 0;
		resources[SOLAR_ID] += timeToMetalDepletion*energyIncome;
		double remainingEnergy = energyCost + energyCostPerSecond*timeToMetalDepletion;
		double remainingTime = remainingMetalToBeUsed/GetProduction(MEX_ID);
		resources[SOLAR_ID] += GetProduction(SOLAR_ID)*remainingTime - remainingEnergy;
		frame += timeToMetalDepletion+remainingTime;
	}
	else if(canBuildVal == -2)
	{
		double remainingEnergyToBeUsed = energyCost - resources[SOLAR_ID] - timeToEnergyDepletion*GetProduction(SOLAR_ID);
		resources[MEX_ID] += timeToEnergyDepletion*metalIncome;
		resources[SOLAR_ID] = 0;
		double remainingMetal = metalCost + metalCostPerSecond*timeToEnergyDepletion;
		double remainingTime = remainingEnergyToBeUsed/GetProduction(SOLAR_ID);
		resources[MEX_ID] += GetProduction(MEX_ID)*remainingTime - remainingMetal;
		frame += timeToEnergyDepletion+remainingTime;
	}
	else //canBuildVal == -3
	{
		if(timeToMetalDepletion < timeToEnergyDepletion)
		{
			double remainingMetalUse = metalCost - resources[MEX_ID] - timeToMetalDepletion*GetProduction(MEX_ID);
			resources[MEX_ID] = 0;
			resources[SOLAR_ID] += timeToMetalDepletion*energyIncome;
			double remainingEnergy = energyCost + energyCostPerSecond*timeToMetalDepletion;
			double remainingTime = remainingMetalUse/GetProduction(MEX_ID);
			double energyProduction = -remainingEnergy/remainingTime + GetProduction(SOLAR_ID);
			double energyDepletion = GetTimeToDepletion(resources[SOLAR_ID], energyProduction);
			if(energyDepletion > remainingTime || energyDepletion == -1)
			{
				resources[SOLAR_ID] += GetProduction(SOLAR_ID)*remainingTime - remainingEnergy;
				frame += timeToMetalDepletion + remainingTime;
			}
			else
			{
				double remainingEnergyUse = remainingEnergy - resources[SOLAR_ID] - timeToEnergyDepletion*GetProduction(SOLAR_ID);
				resources[SOLAR_ID] = 0;
				double remainingMetal = metalCost + metalCostPerSecond*energyDepletion;
				double newRemainingTime = remainingEnergyUse/GetProduction(SOLAR_ID);
				resources[MEX_ID] += GetProduction(MEX_ID)*newRemainingTime - remainingMetal;
				frame += timeToMetalDepletion + remainingTime + newRemainingTime;
			}
		}
		else
		{
			double remainingEnergyUse = energyCost - resources[SOLAR_ID] - timeToEnergyDepletion*GetProduction(SOLAR_ID);
			resources[SOLAR_ID] = 0;
			resources[MEX_ID] += timeToEnergyDepletion*metalIncome;
			double remainingMetal = metalCost + metalCostPerSecond*timeToEnergyDepletion;
			double remainingTime = remainingEnergyUse/GetProduction(SOLAR_ID);
			double metalProduction = -remainingMetal/remainingTime + GetProduction(MEX_ID);
			double metalDepletion = GetTimeToDepletion(resources[MEX_ID], metalProduction);
			if(metalDepletion > remainingTime || metalDepletion == -1)
			{
				resources[MEX_ID] += GetProduction(MEX_ID)*remainingTime - remainingMetal;
				frame += timeToEnergyDepletion + remainingTime;
			}
			else
			{
				double remainingMetalUse = remainingMetal - resources[MEX_ID] - timeToMetalDepletion*GetProduction(MEX_ID);
				resources[MEX_ID] = 0;
				double remainingEnergy = energyCost + energyCostPerSecond*metalDepletion;
				double newRemainingTime = remainingMetalUse/GetProduction(MEX_ID);
				resources[SOLAR_ID] += GetProduction(SOLAR_ID)*newRemainingTime - remainingEnergy;
				frame += timeToEnergyDepletion + remainingTime + newRemainingTime;
			}
		}	
	}
}

short unsigned int Game::GetDiscreteResource(double realValue)
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

double Game::GetAvailableResources(int resourceId, double time)
{
	return resources[resourceId] + GetProduction(resourceId)*time;
}

double Game::GetBuildTime(int unitId, bool commander)
{
	if (unitId == ROCKO_ID)
	{
		return unitDefs[unitId].buildTime/(double)LAB_SPEED;
	} 
	else if(commander)
	{
		return unitDefs[unitId].buildTime/(double)COMMANDER_SPEED;
	}
	else
	{
		return unitDefs[unitId].buildTime/(double)BUILDER_SPEED;
	}
	
}

double Game::BuildingCosts(int resourceId, int buildingID)
{
	if(resourceId == MEX_ID)
	{
		return (double)unitDefs[buildingID].metalCost;
	}
	else
	{
		return (double)unitDefs[buildingID].energyCost;
	}
}

int Game::CanBuild(int unitId )
{
	int retVal = 0;
	double timeToBuild = GetBuildTime(unitId, false);

	double incomeMetal = - (unitDefs[unitId].metalCost)/timeToBuild - GetUsage(MEX_ID);
	double incomeEnergy = - (unitDefs[unitId].energyCost)/timeToBuild - GetUsage(SOLAR_ID);

	double productionMetal = incomeMetal + GetProduction(MEX_ID);
	double productionEnergy = incomeEnergy + GetProduction(SOLAR_ID);
	
	double metalTime = GetTimeToDepletion(resources[MEX_ID], productionMetal);
	double energyTime = GetTimeToDepletion(resources[SOLAR_ID], productionEnergy);

	retVal += (metalTime>=0 && metalTime<timeToBuild ? -1 : 0);
	retVal += (energyTime>=0 && energyTime<timeToBuild ? -2 : 0);

	return retVal;
}

double Game::GetTimeToDepletion(double current, double production)
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

double Game::GetUsage(int resourceId)
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
double Game::GetProduction(int resourceId)
{
	double production = 0;
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


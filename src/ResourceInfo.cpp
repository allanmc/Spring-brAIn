#include "ResourceInfo.h"

using namespace std;
using namespace springai;
using namespace brainSpace;

ResourceInfo::ResourceInfo(AIClasses* ai)
{
	this->ai = ai;
	this->economy = ai->callback->GetEconomy();
	metal = ai->utility->GetResource("Metal");
	energy = ai->utility->GetResource("Energy");
}

ResourceInfo::~ResourceInfo()
{
}

float ResourceInfo::GetCurrentConsumption(Resource *res)
{
	return economy->GetUsage(*res);
}
float ResourceInfo::GetCurrentProduction(Resource *res)
{
	return economy->GetIncome(*res);
}

float ResourceInfo::GetCurrentAvailable(Resource *res)
{
	return economy->GetCurrent(*res);
}
float ResourceInfo::GetCurrentStorage(Resource *res)
{
	return economy->GetStorage(*res);
}


///@return -1 if storage will never get depleted else returns seconds to depletion
float ResourceInfo::GetTimeToDepletion(Resource *resource, float currentProduction)
{
	float storage = economy->GetCurrent(*resource);
	float production = currentProduction;
	//are we already depleted? (very low production means depleted)
	if(storage <= 10 && production < 0.1)
		return 0;

	int frame = ai->frame;
	list<Change>::iterator it;
	for(it = changes.begin(); it != changes.end(); it++)
	{
		if( ((Change)*it).ETA <= frame )
		{
			if ( strcmp(resource->GetName(), "Metal") == 0 )
			{
				production += ((Change)*it).metalProduction;
			}
			else 
			{
				production += ((Change)*it).energyProduction;
			}
		}
		else
		{
			float tmpStorage = storage + production*( ((Change)*it).ETA - frame );
			if(tmpStorage <= 0)
			{
				return (frame - ai->frame + (-storage / production))/AIFRAMES_PR_SECOND;
			}
			else
			{
				storage = tmpStorage;
				frame = ((Change)*it).ETA;
			}
		}
	}

	if(production < 0)
		return (frame - ai->frame + (-storage / production))/AIFRAMES_PR_SECOND;
	else
		return -1;
}
///@return -1 if storage will never get depleted else returns seconds to depletion
float ResourceInfo::GetTimeToMetalDepletion()
{

	float production = economy->GetUsage(*metal) - economy->GetIncome(*metal);
	return GetTimeToDepletion(metal, production);
}

///@return -1 if storage will never get depleted else seconds to depletion
float ResourceInfo::GetTimeToEnergyDepletion()
{
	float production = economy->GetUsage(*energy) - economy->GetIncome(*energy);
	return GetTimeToDepletion(energy, production);
}

///@return Can we afford to build this unit
bool ResourceInfo::IsAffordableToBuild(UnitDef *builder, UnitDef *building)
{
	float timeToBuild = building->GetBuildTime() / builder->GetBuildSpeed();
	float incomeMetal = - (building->GetCost(*metal) / timeToBuild);
	float incomeEnergy = - (building->GetCost(*energy) / timeToBuild);
	float productionMetal = incomeMetal + economy->GetUsage(*metal) - economy->GetIncome(*metal);
	float productionEnergy = incomeEnergy + economy->GetUsage(*energy) - economy->GetIncome(*energy);
	//TODO: Test if we run out of resources before build time is up...
	return	GetTimeToDepletion(metal, productionMetal)>timeToBuild && 
			GetTimeToDepletion(energy, productionEnergy)>timeToBuild;
}


/**
@param ETA the build time in frames
@return an id, to be used for removal
*/
int ResourceInfo::AddChangeToCome(Unit *unit, int ETA)
{
	UnitDef* ud = unit->GetDef();
	Change temp;
	temp.ETA = ai->frame + ETA;
	temp.id = unit->GetUnitId();
	temp.metalProduction = - ud->GetUpkeep(*metal) + ud->GetResourceMake(*metal) + ud->GetExtractsResource(*metal);
	temp.energyProduction = - ud->GetUpkeep(*energy) + ud->GetResourceMake(*energy) + ud->GetExtractsResource(*energy);
	
	ai->utility->ChatMsg("%s has production of (%f,%f)",ud->GetName(),temp.metalProduction,temp.energyProduction);

	list<Change>::iterator it;
	for(it = changes.begin(); it != changes.end(); it++)
	{
		if(((Change)*it).ETA > temp.ETA)
		{
			changes.insert(it,temp);
			break;
		}
	}
	return temp.id;
}
void ResourceInfo::RemoveChangeToCome(Unit *unit)
{
	list<Change>::iterator it;
	ai->utility->ChatMsg("RemoveChangeToCome: %s",unit->GetDef()->GetName());

	for(it = changes.begin(); it != changes.end(); it++)
	{
		if(((Change)*it).id == unit->GetUnitId())
		{
			changes.erase(it);
			break;
		}
	}
}

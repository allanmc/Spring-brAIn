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
float ResourceInfo::GetTimeToMetalDepletion()
{
	float storage = economy->GetCurrent(*metal);
	float production = (economy->GetUsage(*metal) - economy->GetIncome(*metal));
	int frame = ai->frame;
	list<Change>::iterator it;
	for(it = changes.begin(); it != changes.end(); it++)
	{
		if( ((Change)*it).ETA <= frame )
		{
			production += ((Change)*it).metalProduction;
		}
		else
		{
			float tmpStorage = storage + production*( ((Change)*it).ETA - frame );
			if(tmpStorage < 0)
			{
				return (frame + (storage / production))/AIFRAMES_PR_SECOND;
			}
			else
			{
				storage = tmpStorage;
				frame = ((Change)*it).ETA;
			}
		}
	}
	return -1;
}

///@return -1 if storage will never get depleted else seconds to depletion
float ResourceInfo::GetTimeToEnergyDepletion()
{
	float storage = economy->GetCurrent(*energy);
	float production = (economy->GetUsage(*energy) - economy->GetIncome(*energy));
	int frame = ai->frame;
	list<Change>::iterator it;
	for(it = changes.begin(); it != changes.end(); it++)
	{
		if( ((Change)*it).ETA <= frame )
		{
			production += ((Change)*it).energyProduction;
		}
		else
		{
			float tmpStorage = storage + production*( ((Change)*it).ETA - frame );
			if(tmpStorage < 0)
			{
				return (frame + (storage / production))/AIFRAMES_PR_SECOND;
			}
			else
			{
				storage = tmpStorage;
				frame = ((Change)*it).ETA;
			}
		}
	}
	return -1;
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
	
	//ai->utility->ChatMsg("%s has production of (%f,%f)",ud->GetName(),temp.metalProduction,temp.energyProduction);

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
	for(it = changes.begin(); it != changes.end(); it++)
	{
		if(((Change)*it).id == unit->GetUnitId())
		{
			changes.erase(it);
			break;
		}
	}
}

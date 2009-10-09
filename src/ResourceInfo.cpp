#include "ResourceInfo.h"

using namespace std;
using namespace springai;
using namespace brainSpace;

ResourceInfo::ResourceInfo(AIClasses* ai)
{
	this->ai = ai;
	this->economy = ai->callback->GetEconomy();
	currentFrame = 0;
	metal = ai->utility->GetResource("Metal");
	energy = ai->utility->GetResource("Energy");
}

ResourceInfo::~ResourceInfo()
{
}

void ResourceInfo::Update(int frame)
{
	currentFrame = frame;
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

//returns -1 if storage will never get depleted
//else returns seconds to depletion
float ResourceInfo::GetTimeToMetalDepletion()
{
	float storage = economy->GetCurrent(*metal);
	float production = (economy->GetUsage(*metal) - economy->GetIncome(*metal));
	int frame = currentFrame;
	list<Change>::iterator it;
	for(it = changes.begin(); it != changes.end(); it++)
	{
		if( ((Change)*it).ETA <= currentFrame )
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

//returns -1 if storage will never get depleted
//else returns seconds to depletion
float ResourceInfo::GetTimeToEnergyDepletion()
{
	float storage = economy->GetCurrent(*energy);
	float production = (economy->GetUsage(*energy) - economy->GetIncome(*energy));
	int frame = currentFrame;
	list<Change>::iterator it;
	for(it = changes.begin(); it != changes.end(); it++)
	{
		if( ((Change)*it).ETA <= currentFrame )
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

/*
production in amount/sec
ETA, the build time in frames
returns an id, to be used for removal
*/
int ResourceInfo::AddChangeToCome(Unit *unit, int ETA)
{
	UnitDef* ud = unit->GetDef();
	Change temp;
	temp.ETA = currentFrame + ETA;
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

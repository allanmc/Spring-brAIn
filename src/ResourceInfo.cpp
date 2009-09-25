#include "ResourceInfo.h"

ResourceInfo::ResourceInfo(AIClasses* ai)
{
	this->ai = ai;
	this->economy = ai->callback->GetEconomy();
	changeIdCounter = 0;
	currentFrame = 0;
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
float ResourceInfo::GetTimeToDepletion(Resource *res)
{
	float storage = economy->GetCurrent(*res);
	float production = (economy->GetUsage(*res) - economy->GetIncome(*res));
	int frame = currentFrame;
	list<Change>::iterator it;
	for(it = changes.begin(); it != changes.end(); it++)
	{
		if( ((Change)*it).ETA <= currentFrame )
		{
			production += ((Change)*it).production;
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
int ResourceInfo::AddChangeToCome(Resource *res, float production, int ETA)
{
	Change temp;
	temp.ETA = currentFrame + ETA;
	temp.id = ++changeIdCounter;
	temp.res = res;
	temp.production = production;

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
void ResourceInfo::RemoveChangeToCome(int id)
{
	list<Change>::iterator it;
	for(it = changes.begin(); it != changes.end(); it++)
	{
		if(((Change)*it).id == id)
		{
			changes.erase(it);
			break;
		}
	}
}

#include "ResourceInfo.h"

ResourceInfo::ResourceInfo()
{

}

ResourceInfo::~ResourceInfo()
{
}


float ResourceInfo::GetCurrentConsumption(Resource res)
{
	return economy->GetUsage(res);
}
float ResourceInfo::GetCurrentProduction(Resource res)
{
	return economy->GetIncome(res;
}

float ResourceInfo::GetCurrentAvailable(Resource res)
{
	return economy->GetCurrent(res);
}
float ResourceInfo::GetCurrentStorage(Resource res)
{
	return economy->GetStorage(res);
}

float ResourceInfo::GetTimeToDepletion(Resource res)
{
	return economy->GetCurrent()/(economy->GetUsage() - economy->GetIncome());
}

/*
production in amount/sec
ETA in seconds
returns an id, to be used for removal
*/
int ResourceInfo::AddProductionToCome(Resource res, float production, float ETA)
{
	//TODO implement this!
	return -1;
}
void ResourceInfo::RemoveProductionToCome(int id)
{
	//TODO implement this!
}
#include "BuildingController.h"
#include "Resource.h"
#include "UnitDef.h"
using namespace brainSpace;
brainSpace::BuildingController::BuildingController(AICallback* clb)
{
	callback = clb;
}

brainSpace::BuildingController::~BuildingController(void)
{
}

void BuildingController::AddBuilding(springai::Unit *unit)
{
	UnitDef* def = unit->GetDef();
	vector<Resource*> resources = callback->GetResources();
	bool isResource = false;
	for(int i = 0; i < resources.size(); i++)
	{
		if(def->IsResourceMaker(*resources[i]))
			isResource = true;
	}

	if (def->IsAbleToAttack())
	{
		DefenceBuildings.push_back(unit);
	}
	else if (isResource)
	{
		ResourceBuildings.push_back(unit);
	}
	else
	{
		ConstructionBuildings.push_back(unit);
	}

}

void BuildingController::RemoveBuilding(springai::Unit *unit)
{

}

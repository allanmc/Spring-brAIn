#include "BuildingController.h"
#include "Resource.h"

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


void BuildingController::ConstructUnit(SBuildUnitCommand order)
{
	for(int i=0; i<ConstructionBuildings.size(); ++i)
	{
		order.unitId = ConstructionBuildings[i]->GetUnitId();
		callback->GetEngine()->HandleCommand(0,-1,COMMAND_UNIT_BUILD, &order);
	}
}
#include "BuildingController.h"
#include "Resource.h"
#include "UnitDef.h"
#include "Utility.h"

using namespace brainSpace;
brainSpace::BuildingController::BuildingController(AICallback* clb)
{
	callback = clb;
	u = new Utility(clb);
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
		if(def->IsResourceMaker(*resources[i]) || def->GetUpkeep(*resources[i]) < 0 )
		{
			isResource = true;
			break;
		}		
	}

	if (def->GetWeaponMounts().size() > 0)
	{
		DefenceBuildings.push_back(unit);
		u->ChatMsg("Defence building built and added to manager");
	}
	else if (isResource)
	{
		ResourceBuildings.push_back(unit);
		u->ChatMsg("Resource building built and added to manager");
	}
	else
	{
		ConstructionBuildings.push_back(unit);
		u->ChatMsg("Construction building built and added to manager");
	}

}

void BuildingController::RemoveBuilding(springai::Unit *unit)
{

}

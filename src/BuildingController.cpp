#include "BuildingController.h"
//#include "Resource.h"
//#include "UnitDef.h"
//#include "Utility.h"

using namespace brainSpace;
brainSpace::BuildingController::BuildingController( AIClasses* aiClasses )
{
	ai = aiClasses;
}

brainSpace::BuildingController::~BuildingController(void)
{
}

void BuildingController::AddBuilding(springai::Unit *unit)
{
	UnitDef* def = unit->GetDef();
	ai->utility->ChatMsg("Recieved building! type: %s, name: %s",def->GetType(), def->GetName());
	vector<Resource*> resources = ai->callback->GetResources();
	bool isResource = false;
	for(int i = 0; i < resources.size(); i++)
	{
		if(def->IsResourceMaker(*resources[i]) || def->GetUpkeep(*resources[i]) < 0 || def->GetExtractsResource(*resources[i]) > 0
			|| def->GetResourceMake(*resources[i]) > 0)
		{
			isResource = true;
			break;
		}		
	}

	if (def->GetWeaponMounts().size() > 0)
	{
		DefenceBuildings.push_back(unit);
		ai->utility->ChatMsg("Defence building built and added to manager");
	}
	else if (isResource)
	{
		ResourceBuildings.push_back(unit);
		ai->utility->ChatMsg("Resource building built and added to manager");
	}
	else
	{
		ConstructionBuildings.push_back(unit);
		ai->utility->ChatMsg("Construction building built and added to manager");
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
		
		ai->utility->ChatMsg("I (%s) am building: %s",
			Unit::GetInstance(ai->callback, order.unitId)->GetDef()->GetName(),
			UnitDef::GetInstance(ai->callback, order.toBuildUnitDefId)->GetName()
			);
		ai->callback->GetEngine()->HandleCommand(0,-1,COMMAND_UNIT_BUILD, &order);
	}
}

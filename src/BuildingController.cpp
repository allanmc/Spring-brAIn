#include "BuildingController.h"
//#include "Resource.h"
//#include "UnitDef.h"
//#include "Utility.h"

using namespace std;
using namespace springai;
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
	//ai->utility->Log(ALL, MISC, "Recieved building! type: %s, name: %s",def->GetType(), def->GetName());
	vector<Resource*> resources = ai->callback->GetResources();
	bool isResource = false;
	for(int i = 0; i < (int)resources.size(); i++)
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
		//ai->utility->Log(ALL, MISC, "Defence building built and added to manager");
	}
	else if (isResource)
	{
		ResourceBuildings.push_back(unit);
		//ai->utility->Log(ALL, MISC, "Resource building built and added to manager");
	}
	else
	{
		ConstructionBuildings.push_back(unit);
		ai->utility->Log(ALL, MISC, "Construction building built and added to manager");
	}
	delete def;
}

void BuildingController::RemoveBuilding(springai::Unit *unit)
{
	UnitDef* def = unit->GetDef();
	//ai->utility->Log(ALL, MISC, "Recieved building! type: %s, name: %s",def->GetType(), def->GetName());
	vector<Resource*> resources = ai->callback->GetResources();
	bool isResource = false;
	for(int i = 0; i < (int)resources.size(); i++)
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
		for(unsigned int i = 0; i < DefenceBuildings.size(); i++)
		{
			if(unit->GetUnitId() == DefenceBuildings[i]->GetUnitId())
			{
				delete DefenceBuildings[i];
				DefenceBuildings.erase(DefenceBuildings.begin() + i);
				break;
			}
		}
		//ai->utility->Log(ALL, MISC, "Defence building built and added to manager");
	}
	else if (isResource)
	{
		for(unsigned int i = 0; i < ResourceBuildings.size(); i++)
		{
			if(unit->GetUnitId() == ResourceBuildings[i]->GetUnitId())
			{
				delete ResourceBuildings[i];
				ResourceBuildings.erase(ResourceBuildings.begin() + i);
				break;
			}
		}
		//ai->utility->Log(ALL, MISC, "Resource building built and added to manager");
	}
	else
	{
		for(unsigned int i = 0; i < ConstructionBuildings.size(); i++)
		{
			if(unit->GetUnitId() == ConstructionBuildings[i]->GetUnitId())
			{
				delete ConstructionBuildings[i];
				ConstructionBuildings.erase(ConstructionBuildings.begin() + i);
				break;
			}
		}
		ai->utility->Log(ALL, MISC, "Construction building removed from manager");
	}
	delete def;
}

///orders every construction building to contruct the unit
///@param order is the build order containing all information except the contructor
void BuildingController::ConstructUnit(SBuildUnitCommand order)
{
	UnitDef *def = UnitDef::GetInstance(ai->callback, order.toBuildUnitDefId);
	ai->utility->Log(ALL, MISC, "ConstructionBuildings.size: %d",ConstructionBuildings.size());
	for(int i=0; i < (int)ConstructionBuildings.size(); ++i)
	{
		//if((ConstructionBuildings[i]->GetDef()->GetName() == "armlab") && (def->GetName() != "armrock") && (def->GetName() != "armham") && (def->GetName() != "armflea"))
		//{
		//	continue;
		//}
		//if((ConstructionBuildings[i]->GetDef()->GetName() == "armvp") && (def->GetName() != "armfav") && (def->GetName() != "tawf013") && (def->GetName() != "armflash"))
		//{
		//	continue;
		//}
		if(ConstructionBuildings[i]->GetDef() == NULL)
			ai->utility->Log(ALL, MISC, "ConstructionBuildings[i]->GetDef() is NULL!!! :( ");
		ai->utility->Log(ALL, MISC, "ConstructionBuildings[%d]: %s",i, ConstructionBuildings[i]->GetDef()->GetName());
		order.unitId = ConstructionBuildings[i]->GetUnitId();
		order.buildPos = ConstructionBuildings[i]->GetPos();
		//order.facing = UNIT_COMMAND_BUILD_NO_FACING;
		//order.options = UNIT_COMMAND_OPTION_SHIFT_KEY;
		ai->utility->Log(ALL, MISC, "I (%s) am building: %s",
			Unit::GetInstance(ai->callback, order.unitId)->GetDef()->GetName(),
			UnitDef::GetInstance(ai->callback, order.toBuildUnitDefId)->GetName()
			);
		ai->callback->GetEngine()->HandleCommand(0,-1,COMMAND_UNIT_BUILD, &order);
	}
	delete def;
}

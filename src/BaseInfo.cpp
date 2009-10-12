#include "BaseInfo.h"

using namespace std;
using namespace springai;
using namespace brainSpace;

BaseInfo::BaseInfo( AIClasses* aiClasses )
{
	ai = aiClasses;
	CBoundingBox bbox;
	bbox.topLeft = (SAIFloat3){0,0,0};
	bbox.bottomRight = (SAIFloat3){	ai->callback->GetMap()->GetWidth()*8,
									0,
									ai->callback->GetMap()->GetHeight()*8};
	quadTree = new QuadTree( ai, bbox );
	buildingCount = 0;
	resourceBuildings = 0;
	productionBuildings = 0;
	
}

BaseInfo::~BaseInfo()
{
}

void BaseInfo::AddBuilding(Unit* building)
{
	//check for builder units
	if(building->GetDef()->GetSpeed() > 0) return;

	//ai->utility->ChatMsg("Adding base unit...");
	quadTree->InsertUnit(building->GetUnitId(), building->GetPos());

	buildingCount++;
	if(building->GetDef()->IsBuilder())
	{
		productionBuildings++;
	}
	else
	{
		resourceBuildings++;
	}
}

void BaseInfo::RemoveBuilding(Unit* building)
{
	//check for builder units
	if(building->GetDef()->GetSpeed() > 0) return;

	if (buildingCount==0)
	{
		return;
	}
	quadTree->RemoveUnit( building->GetUnitId() );
	//remove unit from quadtree, using pos

	buildingCount--;
	if(building->GetDef()->IsBuilder())
	{
		productionBuildings--;
	}
	else
	{
		resourceBuildings--;
	}
}

///@return the number of resource producing buildings
int BaseInfo::CountResourceBuildings()
{
	return resourceBuildings;
}

///@return the number of buildings that are able to contruct other units
int BaseInfo::CountProductionBuildings()
{
	return productionBuildings;
}

vector<Unit*> BaseInfo::GetUnitsInRange(SAIFloat3 pos, float radius)
{
	SAIFloat3 topLeft = pos;
	SAIFloat3 bottomRight = pos;
	topLeft.x -= radius;
	topLeft.z -= radius;
	bottomRight.x += radius;
	bottomRight.z += radius;

	vector<Unit*> units = quadTree->RangeQuery(topLeft, bottomRight);
	return units;
}

bool BaseInfo::IsBuildingInRange(SAIFloat3 pos, float radius)
{
	vector<Unit*> units = GetUnitsInRange(pos, radius);
	if(units.size() > 0)
	{
		return true;
	}
	return false;
}
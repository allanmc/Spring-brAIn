#ifndef _BRAINSPACE_BUILDINGCONTROLLER_H
#define _BRAINSPACE_BUILDINGCONTROLLER_H
#include "global.h"

namespace brainSpace {

class BuildingController
{
public:
	BuildingController(AICallback* clb);
	~BuildingController(void);
	void AddBuilding(Unit* unit);
	void RemoveBuilding(Unit* unit );
	void ConstructUnit(SBuildUnitCommand order);

private:
	vector<Unit*> DefenceBuildings;
	vector<Unit*> ResourceBuildings;
	vector<Unit*> ConstructionBuildings;
	AICallback* callback;
};
}

#endif // _BRAINSPACE_BUILDINGCONTROLLER_H

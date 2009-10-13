#ifndef _BRAINSPACE_BUILDINGCONTROLLER_H
#define _BRAINSPACE_BUILDINGCONTROLLER_H
#include "global.h"

namespace brainSpace {
///A collection of all our buildings
class BuildingController
{
public:
	BuildingController( AIClasses* aiClasses );
	~BuildingController(void);
	void AddBuilding(Unit* unit);
	void RemoveBuilding(Unit* unit );
	void ConstructUnit(SBuildUnitCommand order);

private:
	vector<Unit*> DefenceBuildings;
	vector<Unit*> ResourceBuildings;
	vector<Unit*> ConstructionBuildings;
	AIClasses* ai;
};
}

#endif // _BRAINSPACE_BUILDINGCONTROLLER_H

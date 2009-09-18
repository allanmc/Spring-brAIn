#ifndef _BRAINSPACE_BUILDINGCONTROLLER_H
#define _BRAINSPACE_BUILDINGCONTROLLER_H
#include "global.h"

using namespace springai;
using namespace std;

namespace brainSpace {

class BuildingController
{
public:
	BuildingController(AICallback* clb);
	~BuildingController(void);
	void AddBuilding(Unit* unit);
	void RemoveBuilding(Unit* unit );

private:
	vector<Unit*> DefenceBuildings;
	vector<Unit*> ResourceBuildings;
	vector<Unit*> ConstructionBuildings;
	AICallback* callback;
	Utility* u;
};
}

#endif // _BRAINSPACE_BUILDINGCONTROLLER_H

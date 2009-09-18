#ifndef _BRAINSPACE_BUILDINGCONTROLLER_H
#define _BRAINSPACE_BUILDINGCONTROLLER_H
#include "Unit.h"
#include <vector>
#include "AICallback.h"

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
};
}

#endif // _BRAINSPACE_BUILDINGCONTROLLER_H

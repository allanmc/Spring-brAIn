#ifndef _BRAINSPACE_BBASEINFO_H
#define _BRAINSPACE_BBASEINFO_H

#include "global.h"
#include "QuadTree.h"

using namespace std;
using namespace springai;

namespace brainSpace
{
	class BaseInfo
	{
	public:
		BaseInfo( AIClasses* aiClasses );
		virtual ~BaseInfo();
		void AddBuilding(Unit* building);
		void RemoveBuilding(Unit* building);

		unsigned int buildingCount;
		
		int CountResourceBuildings();
		int CountProductionBuildings();

	private:
		AIClasses* ai;
		QuadTree* quadTree;
		int resourceBuildings, productionBuildings;
	};
}

#endif

#ifndef _BRAINSPACE_BBASEINFO_H
#define _BRAINSPACE_BBASEINFO_H

#include "global.h"
#include "QuadTree.h"
#include "KMedoids.h"

using namespace std;
using namespace springai;

namespace brainSpace
{
	///Keeps information on a base, both enemy and own.
	class BaseInfo
	{
	public:
		BaseInfo( AIClasses* aiClasses );
		virtual ~BaseInfo();
		void AddBuilding(Unit* building);
		void RemoveBuilding(Unit* building);
		void DrawBasePerimiter();

		unsigned int buildingCount;
		
		int CountResourceBuildings();
		int CountProductionBuildings();
		vector<Unit*> GetUnitsInRange(SAIFloat3 pos, float radius);
		bool IsBuildingInRange(SAIFloat3 pos, float radius);

	private:
		AIClasses* ai;
		QuadTree* quadTree;
		int resourceBuildings, productionBuildings, basePerimiterDrawID;
	};
}

#endif

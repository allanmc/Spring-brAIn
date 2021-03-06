#ifndef _BRAINSPACE_BBASEINFO_H
#define _BRAINSPACE_BBASEINFO_H

#include "global.h"
#include "QuadTree.h"
#include "KMedoids.h"
#include "MilitaryUnitGroup.h"

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
		void RemoveBuilding(int building);
		void DrawBasePerimiter();
		vector< vector<SAIFloat3> > GetBases();
		Unit* GetNearestBaseBuilding();
		Unit* GetWeakestBaseBuilding();
		

		unsigned int buildingCount;
		
		int CountResourceBuildings();
		int CountProductionBuildings();
		int CountBuildingsByName( const char* name );

		vector<Unit*> GetUnitsInRange(SAIFloat3 pos, float radius);
		bool IsBuildingInRange(SAIFloat3 pos, float radius);
		const map<int, struct UnitInformationContainer> GetUnits();
		UnitDef* GetUnitDef(int unitID);
		vector<Unit*> RangeQuery(SAIFloat3 topLeft, SAIFloat3 bottomRight);

	private:
		AIClasses* ai;
		QuadTree* quadTree;
		int resourceBuildings, productionBuildings, basePerimiterDrawID;
	};
}

#endif

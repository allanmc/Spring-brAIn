#ifndef _BRAINSPACE_ARMYINFO_H
#define _BRAINSPACE_ARMYINFO_H


//class QuadTree;

#include "global.h"
#include "QuadTree.h"

using namespace std;
using namespace springai;

namespace brainSpace
{
	class ArmyInfo
	{
	public:
		ArmyInfo( AIClasses* aiClasses );
		virtual ~ArmyInfo();

		void AddUnit(Unit* unit);
		void RemoveUnit(Unit* unit);
		void UpdateUnit(Unit* unit);
		SAIFloat3 GetUnitPos(int unitId);
		void Print();
		vector<Unit*> RangeQuery(float topLeftX, float topLeftZ, float bottomRightX, float bottomRightZ);
		vector<Unit*> RangeQuery(SAIFloat3 topLeft, SAIFloat3 bottomRight);
		vector<Unit*> RangeQuery(CBoundingBox bbox);
		UnitDef* GetUnitDef(int unitID);

		unsigned int unitCount;

		const map<int, struct UnitInformationContainer> GetEnemyUnits();

		int CountDefensive();
		int CountAggressive();

	private:
		AIClasses* ai;
		QuadTree* quadTree;
		map<int,UnitDef*> knownUnitDefs;
		
		int aggressive, defensive;
	};
}

#endif

#ifndef _BRAINSPACE_ARMYINFO_H
#define _BRAINSPACE_ARMYINFO_H


//class QuadTree;

#include "global.h"
#include "QuadTree.h"

using namespace std;
using namespace springai;

namespace brainSpace
{
	///Keeps information on an army, both enemy and own.
	class ArmyInfo
	{
	public:
		ArmyInfo( AIClasses* aiClasses );
		virtual ~ArmyInfo();

		void AddUnit(Unit* unit);
		void RemoveUnit(int unit);
		void UpdateUnit(Unit* unit);
		SAIFloat3 GetUnitPos(int unitId);
		void Print();
		vector<Unit*> RangeQuery(float topLeftX, float topLeftZ, float bottomRightX, float bottomRightZ);
		vector<Unit*> RangeQuery(SAIFloat3 topLeft, SAIFloat3 bottomRight);
		vector<Unit*> RangeQuery(CBoundingBox bbox);
		UnitDef* GetUnitDef(int unitID);

		unsigned int unitCount;

		const map<int, struct UnitInformationContainer> GetUnits();

		int CountDefensive();
		int CountAggressive();
		float GetAggresiveDps();
		float GetDefensiveDps();
		int CountUnitsByName( const char* name );

	private:
		AIClasses* ai;
		QuadTree* quadTree;
		map<int,UnitDef*> knownUnitDefs;
		
		int aggressive, defensive;
		float aggressiveDps, defensiveDps;
	};
}

#endif

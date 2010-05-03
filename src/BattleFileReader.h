#ifndef BATTLE_FILE_READER_H
#define BATTLE_FILE_READER_H

#include "types.h"
#include "MilitaryUnitGroup.h"

#define TOTAL_NUMBER_OF_GAMES 225625

namespace brainSpace
{
	class BattleFileReader
	{
	public:
		BattleFileReader(AIClasses* aiClasses);
		virtual ~BattleFileReader();

		float ReadBattleValue(MilitaryUnitGroup* friendly, MilitaryUnitGroup* enemy);
	private:
		AIClasses* ai;
		std::fstream* bfs;

		void RemoveCommander(std::vector<springai::Unit*> &units);
		int* GetGroupFromUnits(std::vector<int> units);
		int GetClosestGame( MilitaryUnitGroup* friendlyGroup, MilitaryUnitGroup* enemyGroup );
		int GetId(int unitType, int unit1, int unit2, int unit3);
		int SumRange(int max, int min, int num);
		int GetUnitId(int unitType, const char* name);
		int GetNullId(int unitType);
		int CountGroupUnits(std::vector<springai::Unit*> units_old, std::vector<int> &result, bool airGroup);
		int GetUnitId(int unitType, int defId );


		std::vector<int> units;
		std::vector< std::vector<int> > units_all;
	};
}
#endif
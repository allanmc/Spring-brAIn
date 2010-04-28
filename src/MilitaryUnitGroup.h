#ifndef _BRAINSPACE_MILITARY_UNIT_GROUP_H
#define _BRAINSPACE_MILITARY_UNIT_GROUP_H

#include "BrainGroup.h"
#include "types.h"
#include <math.h>

#define MILI_GROUP_MAXSIZE 12
#define MILI_GROUP_MAXTYPES 3

namespace brainSpace
{
	class MilitaryUnitGroup : public BrainGroup
	{
	public:
		enum MilitaryGroupStatus
		{
			MILI_UNIT_GRP_ATTACKING,
			MILI_UNIT_GRP_SCOUTING,
			MILI_UNIT_GRP_IDLE,
			MILI_UNIT_GRP_REGROUPING
		};

		MilitaryUnitGroup( AIClasses* aiClasses, int groupID, bool airGroup );
		virtual ~MilitaryUnitGroup();
		void Guard(int unit);
		MilitaryGroupStatus GetStatus();
		void SetStatus(MilitaryGroupStatus status);
		void Attack(std::vector<int> enemies);
		void Attack(int enemy);
		
		void Scout(SAIFloat3 pos);
		bool UnitIdle(springai::Unit* unit);
		bool IsIdle();

		bool AddUnit( springai::Unit* unit );

		std::map<int, int> GetUnitTypes();
		bool IsAirGroup();

		void StopGroup();

		void FireAtWill();

	private:
		MilitaryGroupStatus Status;

		//UnitDefID, number of units of that type in the group
		std::map<int, int> UnitTypes;
		bool AirGroup;

	};
}

#endif

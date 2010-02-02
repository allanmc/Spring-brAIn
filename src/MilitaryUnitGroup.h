#ifndef _BRAINSPACE_MILITARY_UNIT_GROUP_H
#define _BRAINSPACE_MILITARY_UNIT_GROUP_H

#include "BrainGroup.h"
#include "types.h"
#include <math.h>

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

		MilitaryUnitGroup( AIClasses* aiClasses, int groupID );
		virtual ~MilitaryUnitGroup();
		void Guard(int unit);
		MilitaryGroupStatus GetStatus();
		void SetStatus(MilitaryGroupStatus status);
		void Attack(int enemy);
		void Scout(SAIFloat3 pos);
		void UnitIdle(Unit* unit);
		bool IsIdle();

	private:
		MilitaryGroupStatus Status;
	};
}

#endif

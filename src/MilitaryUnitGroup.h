#ifndef _BRAINSPACE_MILITARY_UNIT_GROUP_H
#define _BRAINSPACE_MILITARY_UNIT_GROUP_H

#include "BrainGroup.h"
#include "global.h"
#include <math.h>

namespace brainSpace
{
	class MilitaryUnitGroup : public BrainGroup
	{
	public:
		enum Status
		{
			Attacking,
			Scouting,
			Idle,
			Regrouping
		};

		MilitaryUnitGroup( AIClasses* aiClasses );
		virtual ~MilitaryUnitGroup();

		Status GetStatus();
		void SetStatus(Status status);
		void Attack(int enemy);
		void Scout(SAIFloat3 pos);
		void UnitIdle(Unit* unit);

	private:
		Status status;
	};
}

#endif

#ifndef _BRAINSPACE_MILITARY_UNIT_GROUP_H
#define _BRAINSPACE_MILITARY_UNIT_GROUP_H

#include "BrainGroup.h"
#include "global.h"
#include "Resource.h"
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

		MilitaryUnitGroup( AICallback* callback );
		virtual ~MilitaryUnitGroup();

		Status GetStatus();
		void SetStatus(Status status);
		void Attack(int enemy);
		void Scout(SAIFloat3 pos);

	private:
		Status status;
	};
}

#endif
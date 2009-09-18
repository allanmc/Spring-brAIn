#ifndef _BRAINSPACE_CONSTRUCTION_UNIT_GROUP_H
#define _BRAINSPACE_CONSTRUCTION_UNIT_GROUP_H

#include "BrainGroup.h"
#include "global.h"

namespace brainSpace
{
	class ConstructionUnitGroup : public BrainGroup
	{
	public:
		ConstructionUnitGroup( AICallback* callback );
		virtual ~ConstructionUnitGroup();

		const vector<UnitDef*> BuildableUnits();
		vector<UnitDef*> IsAbleToBuild();

		bool IsAbleToBuild(UnitDef* unit);
		void AssignBuildOrder( SBuildUnitCommand order );

	private:
	};
}

#endif
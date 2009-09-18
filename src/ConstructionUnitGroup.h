#ifndef _BRAINSPACE_CONSTRUCTION_UNIT_GROUP_H
#define _BRAINSPACE_CONSTRUCTION_UNIT_GROUP_H

#include "BrainGroup.h"

namespace brainSpace
{
	class ConstructionUnitGroup : public BrainGroup
	{
	public:
		ConstructionUnitGroup( );
		virtual ~ConstructionUnitGroup();

		const vector<UnitDef*> BuildableUnits();
		vector<UnitDef*> IsAbleToBuild();
		bool IsAbleToBuild(UnitDef* unit);
	private:
	};
}

#endif
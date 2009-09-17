#ifndef _BRAINSPACE_CONSTRUCTION_UNIT_GROUP_H
#define _BRAINSPACE_CONSTRUCTION_UNIT_GROUP_H

#include "BrainGroup.h"

namespace brainSpace
{
	class ConstructionUnitGroup : public BrainGroup
	{
	public:
		ConstructionUnitGroup( string name );
		virtual ~ConstructionUnitGroup();

		const vector<UnitDef*> BuildableUnits();
		vector<UnitDef*> IsAbleToBuild();
	private:
	};
}

#endif
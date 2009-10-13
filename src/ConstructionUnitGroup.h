#ifndef _BRAINSPACE_CONSTRUCTION_UNIT_GROUP_H
#define _BRAINSPACE_CONSTRUCTION_UNIT_GROUP_H

#include "BrainGroup.h"
#include "global.h"
#include <math.h>

namespace brainSpace
{
	///A group of contruction units
	/**
	 * @warning only support for the commander currently implemented
	 */
	class ConstructionUnitGroup : public BrainGroup
	{
	public:
		ConstructionUnitGroup( AIClasses* aiClasses, int groupID );
		virtual ~ConstructionUnitGroup();

		const vector<UnitDef*> BuildableUnits();
		vector<UnitDef*> IsAbleToBuild();
		
		void SetAvailable();

		bool IsAbleToBuild(UnitDef* unit);
		void AssignBuildOrder( SBuildUnitCommand order );
		void QueueBuildOrder( SBuildUnitCommand order );
	private:
		SAIFloat3 FindClosestMetalExtractionSite( SAIFloat3 pos/*, Resource* metal*/ );
		queue<SBuildUnitCommand> BuildQueue;
		int BaseDefenseCounter;
		int BaseDefenseCounterStart;
		bool BaseDefenseHitBorder;
		UnitDef* metalExtractorUnit;

	};
}

#endif

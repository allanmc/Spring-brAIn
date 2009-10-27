#ifndef _BRAINSPACE_CONSTRUCTION_UNIT_GROUP_H
#define _BRAINSPACE_CONSTRUCTION_UNIT_GROUP_H

#include "BrainGroup.h"
#include "PathfindingMap.h"
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
		bool IsIdle();
		/**
		 *@return A boolean indicating if the group is able to build a specific unit.
		 */
		bool IsAbleToBuild(UnitDef* unit);
		void AssignBuildOrder( SBuildUnitCommand order );
		void QueueBuildOrder( SBuildUnitCommand order );
	private:
		SAIFloat3 FindClosestMetalExtractionSite( SAIFloat3 pos );
		SAIFloat3 FindClosestNonConflictingBuildSite(UnitDef *unitDef, SAIFloat3 buildPos, float searchRadius, int minDist, int facing);
		bool IsMetalExtracitonSite(UnitDef *toBuildUnitDef, SAIFloat3 buildPos);
		bool InersectsWithMex(UnitDef *unitDef, SAIFloat3 pos, SAIFloat3 mexPos);
		queue<SBuildUnitCommand> BuildQueue;
		int BaseDefenseCounter;
		int BaseDefenseCounterStart;
		bool BaseDefenseHitBorder;
		SAIFloat3 safePosition;
		SAIFloat3 GetSafePosition();
		bool BuildBlocksSelf(UnitDef *unitDef, SAIFloat3 pos, int facing);
		SAIFloat3 FindBestDefensePosition(UnitDef *unitDef, SAIFloat3 buildPos);
		SAIFloat3 GetUnitExitOfLab(SAIFloat3 centerPos, UnitDef *unitDef, int facing);
		SAIFloat3 FindGoodBuildSite(SAIFloat3 builderPos, UnitDef* building, float radius);

	};
}

#endif

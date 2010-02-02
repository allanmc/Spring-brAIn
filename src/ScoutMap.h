#ifndef _BRAINSPACE_SCOUTMAP_H
#define _BRAINSPACE_SCOUTMAP_H

#include "types.h"
#include "BrainMap.h"

namespace brainSpace {

	///A map with information on when we have seen different places.
	/**
	 * Keeps up to date timestamps telling when we last saw a cell on the map.
	 * Used by scouts, to determine where scouting is needed.
	 */
	class ScoutMap : public BrainMap
	{
	public:
		ScoutMap( AIClasses* aiClasses);
		virtual ~ScoutMap();
		float GetScoutedPercentageWithinSeconds(int i);

		void Update();

	private:
		void EffectCell(int index, float value);
	};
}
#endif
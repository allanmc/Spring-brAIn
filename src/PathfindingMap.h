#ifndef _BRAINSPACE_PATHFINDINGMAP_H
#define _BRAINSPACE_PATHFINDINGMAP_H

#include "types.h"
#include "BrainMap.h"

namespace brainSpace {

	///A map with information on when we have seen different places.
	/**
	 * Keeps up to date timestamps telling when we last saw a cell on the map.
	 * Used by scouts, to determine where scouting is needed.
	 */
	class PathfindingMap : public BrainMap
	{
	public:
		PathfindingMap( AIClasses* aiClasses );
		virtual ~PathfindingMap();

		void Update();
	private:
		void EffectCell(int index, float value);
	};
}

#endif
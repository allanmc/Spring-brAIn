#ifndef _BRAINSPACE_MAPINFO_H
#define _BRAINSPACE_MAPINFO_H

#include "types.h"
#include "ThreatMap.h"
#include "ScoutMap.h"
#include "BrainResourceMap.h"

namespace brainSpace
{
	///A collection of all the classes that provide information about the map.
	class MapInfo
	{
	public:
		MapInfo( AIClasses* aiClasses );
		virtual ~MapInfo();
		ThreatMap *threatMap;
		ScoutMap *scoutMap;
		BrainResourceMap* resourceMap;

	protected:

	private:
		AIClasses* ai;
		

	};
}
#endif

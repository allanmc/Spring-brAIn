#ifndef _BRAINSPACE_MAPINFO_H
#define _BRAINSPACE_MAPINFO_H

#include "types.h"
#include "ThreatMap.h"
#include "ScoutMap.h"

namespace brainSpace
{
	class MapInfo
	{
	public:
		MapInfo( AIClasses* aiClasses );
		virtual ~MapInfo();
		ThreatMap *threatMap;
		ScoutMap *scoutMap;

	protected:

	private:
		AIClasses* ai;
		

	};
}
#endif

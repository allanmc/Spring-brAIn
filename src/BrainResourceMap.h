#ifndef _BRAINSPACE_BRAINRESOURCEMAP_H
#define _BRAINSPACE_BRAINRESOURCEMAP_H

#include "types.h"
#include "BrainMap.h"

namespace brainSpace {
	
	///A map telling how many metal spots there are in each cell, used for scouting
	class BrainResourceMap : public BrainMap
	{
	public:
		BrainResourceMap( AIClasses* aiClasses );
		virtual ~BrainResourceMap();

		void Update();

		void EffectCell(int index, float value);
	};
}

#endif

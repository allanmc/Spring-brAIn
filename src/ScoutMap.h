#ifndef _BRAINSPACE_SCOUTMAP_H
#define _BRAINSPACE_SCOUTMAP_H

#include "types.h"
#include "BrainMap.h"

namespace brainSpace {

	class ScoutMap : public BrainMap
	{
	public:
		ScoutMap( AIClasses* aiClasses);
		virtual ~ScoutMap();

		void Update();

	private:
		void EffectCell(int index, float value);
	};
}
#endif
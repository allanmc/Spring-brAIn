#ifndef _BRAINSPACE_THREATMAP_H
#define _BRAINSPACE_THREATMAP_H

#include "global.h"

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
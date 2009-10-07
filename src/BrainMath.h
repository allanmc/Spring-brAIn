#ifndef _BRAINSPACE_BRAINMATH_H
#define _BRAINSPACE_BRAINMATH_H

#include "global.h"
#include "QuadTreeNode.h"
namespace brainSpace 
{
	class BrainMath
	{
	public:

		AIClasses* ai;
		BrainMath( AIClasses* aiClasses )
		{
			ai = aiClasses;
		}
		bool CircleIntersectBoundingBox( CBoundingBox box, SAIFloat3 center, float radius );
	};
}

#endif
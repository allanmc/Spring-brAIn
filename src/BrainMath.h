#ifndef _BRAINSPACE_BRAINMATH_H
#define _BRAINSPACE_BRAINMATH_H

#include "global.h"
#include "QuadTreeNode.h"
#include <algorithm>

//static bool cmpSAIFloat3 (SAIFloat3 P1, SAIFloat3 P2);

namespace brainSpace 
{
	///A collection of Math fuctions frequently used
	class BrainMath
	{
	public:

		AIClasses* ai;
		BrainMath( AIClasses* aiClasses )
		{
			ai = aiClasses;
		}
		bool CircleIntersectBoundingBox( CBoundingBox box, SAIFloat3 center, float radius );
		bool CircleIntersetCircle(SAIFloat3 center1, float radius1, SAIFloat3 center2, float radius2);  
		std::vector<SAIFloat3> Sort(std::vector<SAIFloat3> points);
		bool BoxIntersect(SAIFloat3 pos1, float width1, float height1, SAIFloat3 pos2, float width2, float height2);
	};
	

}

#endif
#ifndef _CONVEXHULL_H
#define _CONVEXHULL_H

#include "global.h"
#include <algorithm>


///A collection of Math fuctions frequently used
namespace brainSpace {
	class ConvexHull
	{
		private:
			float isLeft( SAIFloat3 P0, SAIFloat3 P1, SAIFloat3 P2 );
			AIClasses* ai;
		public:
			ConvexHull( AIClasses* aiClasses )
			{
				ai = aiClasses;
			}
			virtual ~ConvexHull(){}
			int chainHull_2D( SAIFloat3* P, int n, SAIFloat3* H );

	};
}

#endif //_CONVEXHULL_H
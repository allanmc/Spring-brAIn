#ifndef _BRAINSPACE_TESTCASE_H
#define _BRAINSPACE_TESTCASE_H


#include "types.h"

namespace brainSpace {
	
	class TestCase
	{
	public:
		TestCase( AIClasses* aiClasses );
		virtual ~TestCase();

	private:
		AIClasses* ai;

		void MakeMetalExtractors( int tileX, int tileZ, int count );
		void MakeThreat( int tileX, int tileZ, bool equal = false );

	};

}
#endif
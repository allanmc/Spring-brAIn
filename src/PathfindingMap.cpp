#include "PathfindingMap.h"
#include "Knowledge.h"

using namespace std;
using namespace springai;
using namespace brainSpace;

PathfindingMap::PathfindingMap( AIClasses* aiClasses ) : BrainMap( aiClasses, 4 )
{
	
	//Slopemap must be iterated height-first
	vector<float> slopeMap = ai->callback->GetMap()->GetSlopeMap();
	int width = ai->callback->GetMap()->GetWidth()/2;
	int height = ai->callback->GetMap()->GetHeight()/2;

	for ( int x = 0 ; x < width ; x++ )
		for ( int z = 0 ; z < height ; z++ )
		{
			ai->utility->Log( ALL, SLOPEMAP, "(%d, %d): %f", x, z, slopeMap[z*width + x] );
		}
}


PathfindingMap::~PathfindingMap()
{
}


void PathfindingMap::EffectCell(int index, float value)
{
}
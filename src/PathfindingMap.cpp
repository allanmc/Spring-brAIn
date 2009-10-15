#include "PathfindingMap.h"
#include "Knowledge.h"

using namespace std;
using namespace springai;
using namespace brainSpace;

PathfindingMap::PathfindingMap( AIClasses* aiClasses ) : BrainMap( aiClasses, 4 )
{
	
	//Slopemap has Resolution = 2, where PFmap has Resolution = 4.
	vector<float> slopeMap = ai->callback->GetMap()->GetSlopeMap();
	int width = ai->callback->GetMap()->GetWidth()/2;
	int height = ai->callback->GetMap()->GetHeight()/2;

	for ( int i = 0 ; i < MapWidth*MapHeight ; i++ )
	{
		float slope1 = slopeMap[i*2];
		float slope2 = slopeMap[(i*2)+1];
		float slope3 = slopeMap[(i*2)+width];
		float slope4 = slopeMap[(i*2)+width+1];
		//ai->utility->Log( ALL, SLOPEMAP, "Index %d. Slope1: %f, Slope2: %f, Slope3: %f, Slope4: %f ", i, slope1, slope2, slope3, slope4 );  
		MapArray[i] = max( slope4, max( slope3, max( slope1, slope2 ) ) );
		//ai->utility->Log( ALL, SLOPEMAP, "MapArray %d: %f", i, MapArray[i] );
	}

	
	for ( int x = 0 ; x < width ; x++ )
		for ( int z = 0 ; z < height ; z++ )
		{
			//ai->utility->Log( ALL, SLOPEMAP, "Slopemap: (%d, %d): %f", x, z, slopeMap[z*width + x] );
		}
		
}


PathfindingMap::~PathfindingMap()
{
}


void PathfindingMap::EffectCell(int index, float value)
{
}
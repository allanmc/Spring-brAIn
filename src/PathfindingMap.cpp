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

	for ( int z = 0 ; z < MapHeight ; z++ )
		for ( int x = 0 ; x < MapWidth ; x++ )
		{
			float slope1 = slopeMap[x*2+width*2*z];
			float slope2 = slopeMap[((x*2)+1)+width*2*z];
			float slope3 = slopeMap[(x*2)+width*2*z+width];
			float slope4 = slopeMap[((x*2)+1)+width*2*z+width];
			ai->utility->Log( ALL, SLOPEMAP, "Index %d. Slope1: %f, Slope2: %f, Slope3: %f, Slope4: %f ", z*MapWidth + x, slope1, slope2, slope3, slope4 );  
			MapArray[z*MapWidth + x] = max( slope4, max( slope3, max( slope1, slope2 ) ) );
			ai->utility->Log( ALL, SLOPEMAP, "MapArray %d: %f", z*MapWidth + x, MapArray[z*MapWidth + x] );
		}

		for ( int z = 0 ; z < height ; z++ )
			for ( int x = 0 ; x < width ; x++ )
			{
				ai->utility->Log( ALL, SLOPEMAP, "Slopemap: (%d, %d): %f", x, z, slopeMap[z*width + x] );
			}

}


PathfindingMap::~PathfindingMap()
{
}


void PathfindingMap::EffectCell(int index, float value)
{
}


void PathfindingMap::Update()
{
	ai->utility->RemoveGraphics(FigureID);
	FigureID = 0;

	for ( int j = 0 ; j < MapHeight ; j++ )
		for ( int i = 0 ; i < MapWidth ; i++ )
		{
			if ( MapArray[ j*MapWidth + i ] > 0.45f )
			{
				SAIFloat3 start, end;
				start.x = i*Resolution;
				start.y = 100;
				start.z = j*Resolution + 0.5*Resolution;

				end.x = (i+1)*Resolution;
				end.y = 100;
				end.z = j*Resolution + 0.5*Resolution;


				int id = ai->utility->DrawLine( start, end, false, Resolution*2, FigureID );
				if(FigureID == 0) FigureID = id;
			}
		}
}
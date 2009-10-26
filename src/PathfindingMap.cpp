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
			//ai->utility->Log( ALL, SLOPEMAP, "Index %d. Slope1: %f, Slope2: %f, Slope3: %f, Slope4: %f ", z*MapWidth + x, slope1, slope2, slope3, slope4 );  
			MapArray[z*MapWidth + x] = max( slope4, max( slope3, max( slope1, slope2 ) ) );
			//ai->utility->Log( ALL, SLOPEMAP, "MapArray %d: %f", z*MapWidth + x, MapArray[z*MapWidth + x] );
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
	ai->utility->Log( ALL, SLOPEMAP, "Updating PFmap" );
	ai->utility->RemoveGraphics(FigureID);
	FigureID = 0;

	for ( int j = 0 ; j < MapHeight ; j++ )
		for ( int i = 0 ; i < MapWidth ; i++ )
		{
			if ( MapArray[ j*MapWidth + i ] == 10.0f )
			{
				//ai->utility->Log( ALL, SLOPEMAP, "(%d, %d) is equal to 10", i, j );
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

void PathfindingMap::AddBuilding(int unitID)
{
	Unit* u = Unit::GetInstance( ai->callback, unitID );
	SAIFloat3 pos = u->GetPos();
	int centerCellX = pos.x/Resolution;
	int centerCellZ = pos.z/Resolution;
	int xSize = u->GetDef()->GetXSize();
	int zSize = u->GetDef()->GetZSize();
	int topCell = (pos.z-zSize)/Resolution;
	int bottomCell = (pos.z+zSize)/Resolution;
	int leftCell = (pos.x-xSize)/Resolution;
	int rightCell = (pos.x+xSize)/Resolution;


	for ( int i = topCell ; i <= bottomCell ; i++ )
		for ( int j = leftCell ; j <= rightCell ; j++ )
		{
			if ( i > MapHeight || i < 0 || j > MapWidth || j < 0 )
				continue;

			MapArray[i*MapWidth + j] = 10.0f; //Now this tile is impassable.
			ai->utility->Log( ALL, SLOPEMAP, "Unit: %s", u->GetDef()->GetHumanName() );
			ai->utility->Log( ALL, SLOPEMAP, "Tile ( %d, %d ) marked as impassable", j, i );
			ai->utility->Log( ALL, SLOPEMAP, "Center: (%f, %f)", pos.x, pos.z );
			ai->utility->Log( ALL, SLOPEMAP, "Top: %d, Bottom: %d, Left: %d, Right: %d, Center: ( %d, %d )", topCell, bottomCell, leftCell, rightCell, centerCellX, centerCellZ );
			ai->utility->Log( ALL, SLOPEMAP, "MapWidth %d. MapHeight: %d", MapWidth, MapHeight );
		}
}

void PathfindingMap::RemoveBuilding(int unitID)
{

}
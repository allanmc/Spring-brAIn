#include "BrainMap.h"

using namespace std;
using namespace springai;
using namespace brainSpace;


BrainMap::BrainMap( AIClasses* aiClasses )
{
	ai = aiClasses;
	Resolution = 64*8; //real world size of a map tile

	int width = ai->callback->GetMap()->GetWidth();
	int height = ai->callback->GetMap()->GetHeight();

	MapWidth = (ai->callback->GetMap()->GetWidth()*8)/Resolution;
	MapHeight = (ai->callback->GetMap()->GetHeight()*8)/Resolution;

	MapArray = new float[MapWidth*MapHeight];
	mapInfo = new MapInfo(MapArray, MapWidth, MapHeight);
	FigureID = 0;
	GridFigureID = 0;

	DrawGrid();
	Reset();
}


BrainMap::~BrainMap()
{
	delete(MapArray);
}

void BrainMap::Update()
{
}

const MapInfo* BrainMap::GetMapInfo()
{
	return mapInfo;
}

void BrainMap::EffectCircle( SAIFloat3 center, float radius, float value, bool additive )
{
	int currentIndexX = floorf(center.x/Resolution );
	int currentIndexZ = floorf(center.z/Resolution);
	int range = radius/Resolution;

	for ( int i = currentIndexX-range ; i <= currentIndexX+range ; i++ )
	{
		for ( int j = currentIndexZ-range ; j <= currentIndexZ+range ; j++ )
		{
			if ( i < 0 || j < 0 || i >= MapWidth || j >= MapHeight )
				continue;
			CBoundingBox b;
			b.topLeft.x = i*Resolution;
			b.topLeft.z = j*Resolution;
			b.bottomRight.x = (i+1)*Resolution;
			b.bottomRight.z = (j+1)*Resolution;
			if ( ai->math->CircleIntersectBoundingBox( b, center, radius ) )
			{
				if(additive)
					MapArray[ j*MapWidth + i ] += value;
				else
					MapArray[ j*MapWidth + i ] = value;
			}
		}
	}
}


void BrainMap::Reset()
{
	for ( int i = 0 ; i < MapWidth*MapHeight ; i++ )
		MapArray[i] = 0;
}

void BrainMap::DrawGrid()
{
	ai->utility->RemoveGraphics( GridFigureID );
	GridFigureID = 0;
	for ( int i = 0 ; i < MapHeight ; i++ )
	{
		SAIFloat3 start, end;
		start.x = 0;
		start.y = 200;
		start.z = i*Resolution;
		end.x = MapWidth*Resolution;
		end.y = 200;
		end.z = i*Resolution;
		int id = ai->utility->DrawLine( start, end, false, 20, GridFigureID );
		if(GridFigureID == 0) GridFigureID = id;
	}

	for ( int i = 0 ; i < MapWidth ; i++ )
	{		
		SAIFloat3 start, end;
		start.x = i*Resolution;
		start.y = 200;
		start.z = 0;
		end.x = i*Resolution;
		end.y = 200;
		end.z = MapHeight*Resolution;

		int id = ai->utility->DrawLine( start, end, false, 20 , GridFigureID );
	}
}
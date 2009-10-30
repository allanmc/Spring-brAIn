#include "BrainMap.h"

using namespace std;
using namespace springai;
using namespace brainSpace;


BrainMap::BrainMap( AIClasses* aiClasses, int resolution )
{
	ai = aiClasses;
	Resolution = resolution*8; //real world size of a map tile

	MapWidth = (ai->callback->GetMap()->GetWidth()*8)/Resolution;
	MapHeight = (ai->callback->GetMap()->GetHeight()*8)/Resolution;

	MapArray = new float[MapWidth*MapHeight];
	mapData = new MapData(MapArray, MapWidth, MapHeight, Resolution);
	FigureID = 0;
	GridFigureID = 0;

	//DrawGrid();
	Reset();
}


BrainMap::~BrainMap()
{
	delete[](MapArray);
	delete(mapData);
}

///Should be overwrited in sub-classes and be used for keeping the map up to date.
void BrainMap::Update()
{
}

///@return The MapData struct
///@see MapData
const MapData* BrainMap::GetMapData()
{
	return mapData;
}

///Applies EffectCell on all tiles that intersect with the cirle
///@see EffectCell()
void BrainMap::EffectCircle( SAIFloat3 center, float radius, float value )
{
	int currentIndexX = floorf(center.x/Resolution );
	int currentIndexZ = floorf(center.z/Resolution);
	int range = ceil(radius/Resolution);//Ceil to make sure that it checks border cells even though the range is not a whole cell
	
	EffectCell(currentIndexZ * MapWidth + currentIndexX, value);//center cell
	for(int x = max(currentIndexX - range,0); x<currentIndexX; x++)//left cells
	{
		if((x+1)*Resolution > center.x - radius)//right edge check
			EffectCell(currentIndexZ*MapWidth + x, value);
	}
	for(int x = min(currentIndexX + range,MapWidth-1); x>currentIndexX; x--)//right cells
	{
		if(x*Resolution < center.x + radius)//left edge check
			EffectCell(currentIndexZ*MapWidth + x, value);
	}
	for(int z = max(currentIndexZ - range,0); z<currentIndexZ; z++)//bottom cells
	{
		if((z+1)*Resolution > center.z - radius)//top edge check
			EffectCell(z*MapWidth + currentIndexX, value);
	}
	for(int z = min(currentIndexZ + range,MapHeight-1); z>currentIndexZ; z--)//top cells
	{
		if(z*Resolution < center.z + radius)
			EffectCell(z*MapWidth + currentIndexX, value);
	}
	
	SAIFloat3 cornor;
	for ( int x = currentIndexX-range ; x < currentIndexX ; x++ )//top left cells
	{
		cornor.x = (x+1)*Resolution;//right
		for ( int z = currentIndexZ-range ; z < currentIndexZ ; z++ )
		{
			cornor.z = (z+1)*Resolution;//bottom
			if(ai->utility->EuclideanDistance(center,cornor) < radius)//bottom right conor check
				EffectCell(z*MapWidth + x, value);
		}
	}
	for ( int x = currentIndexX+range ; x > currentIndexX ; x-- )//top right cells
	{
		cornor.x = x*Resolution;//left
		for ( int z = currentIndexZ-range ; z < currentIndexZ ; z++ )
		{
			cornor.z = (z+1)*Resolution;//bottom
			if(ai->utility->EuclideanDistance(center,cornor) < radius)//bottom left conor check
				EffectCell(z*MapWidth + x, value);
		}
	}
	for ( int x = currentIndexX-range ; x < currentIndexX ; x++ )//bottom left cells
	{
		cornor.x = (x+1)*Resolution;//right
		for ( int z = currentIndexZ+range ; z > currentIndexZ ; z-- )
		{
			cornor.z = z*Resolution;//top
			if(ai->utility->EuclideanDistance(center,cornor) < radius)//top right conor check
				EffectCell(z*MapWidth + x, value);
		}
	}
	for ( int x = currentIndexX+range ; x > currentIndexX ; x-- )//top left cells
	{
		cornor.x = x*Resolution;//left
		for ( int z = currentIndexZ+range ; z > currentIndexZ ; z-- )
		{
			cornor.z = z*Resolution;//top
			if(ai->utility->EuclideanDistance(center,cornor) < radius)//top left conor check
				EffectCell(z*MapWidth + x, value);
		}
	}
}

///Sets all cells to zero
void BrainMap::Reset()
{
	for ( int i = 0 ; i < MapWidth*MapHeight ; i++ )
		MapArray[i] = 0;
}

///draws the grid on the map
void BrainMap::DrawGrid()
{
	ai->utility->RemoveGraphics( GridFigureID );
	GridFigureID = 0;
	for ( int i = 0 ; i < MapHeight ; i++ )
	{
		SAIFloat3 start, end;
		start.x = 0;
		start.y = 50;
		start.z = i*Resolution;
		end.x = MapWidth*Resolution;
		end.y = 50;
		end.z = i*Resolution;
		int id = ai->utility->DrawLine( start, end, false, 5, GridFigureID );
		if(GridFigureID == 0) GridFigureID = id;
	}

	for ( int i = 0 ; i < MapWidth ; i++ )
	{		
		SAIFloat3 start, end;
		start.x = i*Resolution;
		start.y = 50;
		start.z = 0;
		end.x = i*Resolution;
		end.y = 50;
		end.z = MapHeight*Resolution;

		ai->utility->DrawLine( start, end, false, 5 , GridFigureID );
	}
}

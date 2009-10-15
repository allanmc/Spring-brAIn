#ifndef _BRAINSPACE_BRAINMAP_H
#define _BRAINSPACE_BRAINMAP_H

#include "types.h"

namespace brainSpace {
///The MapData-struct
/**
A data struct used as a return value from the "Map*"-classes, it holds all information nessasary about a map.
*/
struct MapData
{
	///The raw map-data.
	float *MapArray;
	///The width of the map in cells.
	int MapWidth;
	///The height of the map in cells.
	int MapHeight;
	///The the height and width of a cell in position values.
	int MapResolution;

	MapData(float *map, int width, int height, int resolution)
	{
		MapArray = map;
		MapWidth = width;
		MapHeight = height;
		MapResolution = resolution;
	}
};

///The general map, with all basic actions implemented.
/**
@warning can not be instanciated with out inheritance.
*/
class BrainMap
{
public:
	///Resolution is in map units, not real-world units
	BrainMap( AIClasses* aiClasses, int resolution = 64 );
	virtual ~BrainMap();

	virtual void Update();
	const MapData *GetMapData();
	void DrawGrid();

protected:

	int Resolution;
	int MapWidth;
	int MapHeight;

	float* MapArray;
	const MapData *mapData;
	int FigureID;
	int GridFigureID;
	
	
	void Reset();
	void EffectCircle( SAIFloat3 center, float radius, float value );
	virtual void EffectCell(int index, float value)=0;
	AIClasses* ai;
};
}
#endif
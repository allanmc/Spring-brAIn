#ifndef _BRAINSPACE_BRAINMAP_H
#define _BRAINSPACE_BRAINMAP_H

#include "types.h"

namespace brainSpace {
struct MapData
{
	float *MapArray;
	int MapWidth;
	int MapHeight;

	MapData(float *map, int width, int height)
	{
		MapArray = map;
		MapWidth = width;
		MapHeight = height;
	}
};

class BrainMap
{
public:
	BrainMap( AIClasses* aiClasses);
	virtual ~BrainMap();

	virtual void Update();
	const MapData *GetMapData();

protected:

	int Resolution;
	int MapWidth;
	int MapHeight;

	float* MapArray;
	const MapData *mapData;
	int FigureID;
	int GridFigureID;
	
	void DrawGrid();
	void Reset();
	void EffectCircle( SAIFloat3 center, float radius, float value );
	virtual void EffectCell(int index, float value)=0;
	AIClasses* ai;
};
}
#endif
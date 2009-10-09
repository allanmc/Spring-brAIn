#ifndef _BRAINSPACE_BRAINMAP_H
#define _BRAINSPACE_BRAINMAP_H

#include "global.h"

namespace brainSpace {
struct MapInfo
{
	float *MapArray;
	int MapWidth;
	int MapHeight;

	MapInfo(float *map, int width, int height)
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
	const MapInfo *GetMapInfo();

protected:

	int Resolution;
	int MapWidth;
	int MapHeight;

	float* MapArray;
	const MapInfo *mapInfo;
	int FigureID;
	int GridFigureID;
	
	void DrawGrid();
	void Reset();
	void EffectCircle( SAIFloat3 center, float radius, float value, bool additive );
	void SetValue(int index, float value);
	AIClasses* ai;
};
}
#endif
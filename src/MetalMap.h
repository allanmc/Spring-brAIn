#ifndef _BRAINSPACE_METALMAP_H
#define _BRAINSPACE_METALMAP_H

#include "global.h"
#include "Resource.h"
#include <math.h>
#include "Map.h"

class MetalMap
{
public:
	MetalMap( AICallback* callback );
	virtual ~MetalMap();
	void Init();

private:

	void GetMetalPoints();
	void SaveMetalMap();
	bool LoadMetalMap();


	AICallback* Callback;

	Resource* Metal;


	std::vector<SAIFloat3> VectoredSpots;
	SAIFloat3 BufferSpot;
	int NumSpotsFound;
	float AverageMetal;
	bool Stopme;
	int MaxSpots;
	int MetalMapHeight;
	int MetalMapWidth;
	int TotalCells;
	int SquareRadius;
	int DoubleSquareRadius;
	int TotalMetal;
	int MaxMetal;
	int TempMetal;
	int coordx;
	int coordy;
	int Minradius;
	int MinMetalForSpot;
	int XtractorRadius;
	int DoubleRadius;
	unsigned char* MexArrayA;
	unsigned char* MexArrayB;
	unsigned char* MexArrayC;
	int* TempAverage;


};

#endif


#ifndef _BRAINSPACE_METALMAP_H
#define _BRAINSPACE_METALMAP_H

#include "global.h"
class MetalMap
{
public:
	MetalMap( AICallback* callback );
	virtual ~MetalMap();


private:

	AICallback* m_callback;
	
	
	void GetMetalPoints();

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


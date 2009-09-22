#include "MetalMap.h"

MetalMap::MetalMap( AICallback* callback )
{
	m_callback = callback;
	// from 0-255, the minimum percentage of metal a spot needs to have from
	// the maximum to be saved, prevents crappier spots in between taken spaces
	// (they are still perfectly valid and will generate metal mind you!)
	MinMetalForSpot = 50;
	// if more spots than that are found the map is considered a metalmap, tweak this as needed
	MaxSpots = 10000;

	// metal map has 1/2 resolution of normal map
	MetalMapHeight = m_callback->GetMap()->GetHeight() / 2;
	MetalMapWidth = m_callback->GetMap()->GetWidth() / 2;

	TotalCells = MetalMapHeight * MetalMapWidth;
	XtractorRadius = int(m_callback->GetMap()->GetExtractorRadius() / 16);
	DoubleRadius = XtractorRadius * 2;
	SquareRadius = XtractorRadius * XtractorRadius;
	DoubleSquareRadius = DoubleRadius * DoubleRadius;

	MexArrayA = new unsigned char [TotalCells];
	MexArrayB = new unsigned char [TotalCells];
	// used for drawing the TGA, not really needed with a couple of changes
	MexArrayC = new unsigned char [TotalCells];

	TempAverage = new int [TotalCells];
	TotalMetal = MaxMetal = NumSpotsFound = 0;
	Stopme = false;

	GetMetalPoints();
}

MetalMap::~MetalMap()
{
}


MetalMap::GetMetalPoints()
{
	int* xend = new int[DoubleRadius + 1];

	for (int a = 0; a < DoubleRadius + 1; a++) {
		float z = a - XtractorRadius;
		float floatsqrradius = SquareRadius;
		xend[a] = int(sqrtf(floatsqrradius - z * z));
	}

	// load up the metal values in each pixel
	Resource* metal;
	for ( int i = 0 ; i < m_callback->GetResources() ; i++ )
	{
		if ( strcmp( m_callback->GetResources()[i], "Metal" ) == 0 )
		{
			metal = m_callback->GetResources()[i];
			
		}
	}
	const unsigned char* metalMapArray = m_callback->GetMap()->GetResourceMapRaw();
	double TotalMetalDouble  = 0;

	for (int i = 0; i < TotalCells; i++) {
		// count the total metal so you can work out an average of the whole map
		TotalMetalDouble +=  MexArrayA[i] = metalMapArray[i];
	}

	// do the average
	AverageMetal = TotalMetalDouble / TotalCells;
}
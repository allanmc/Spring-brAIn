#ifndef _BRAINSPACE_THREATMAP_H
#define _BRAINSPACE_THREATMAP_H


#include "global.h"
#include "WeaponMount.h"
#include "WeaponDef.h"
#include "Damage.h"

namespace brainSpace {
class ThreatMap
{
public:
	ThreatMap( AIClasses* aiClasses);
	virtual ~ThreatMap();


	void Update();


private:

	int ArmorType;
	int ThreatMapResolution;
	int ThreatMapWidth;
	int ThreatMapHeight;
	float CellWidth;
	float CellHeight;

	float* ThreatArray;
	int* FigureIDs;
	int* GridFigureIDs;
	
	void DrawGrid();
	void Reset();
	void InsertUnit( springai::Unit* u, struct UnitInformationContainer c );
	int CalculateDPS( springai::WeaponDef* w );

	AIClasses* ai;
};
}
#endif
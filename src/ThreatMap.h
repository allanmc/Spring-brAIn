#ifndef _BRAINSPACE_THREATMAP_H
#define _BRAINSPACE_THREATMAP_H


#include "global.h"
#include "WeaponMount.h"
#include "WeaponDef.h"
#include "Damage.h"

class ThreatMap
{
public:
	ThreatMap( AIClasses* aiClasses);
	virtual ~ThreatMap();


	void Update();


private:

	int ThreatMapResolution;
	int ThreatMapWidth;
	int ThreatMapHeight;
	float CellWidth;
	float CellHeight;

	float ThreatArray[];
	void InsertUnit( Unit* u );

	AIClasses* ai;
};
#endif
#ifndef _BRAINSPACE_THREATMAP_H
#define _BRAINSPACE_THREATMAP_H


#include "global.h"
#include "WeaponMount.h"
#include "WeaponDef.h"
#include "Damage.h"

namespace brainSpace {
class ThreatMap : public BrainMap
{
public:
	ThreatMap( AIClasses* aiClasses);
	virtual ~ThreatMap();
	void Update();
private:

	int ArmorType;
	
	void InsertUnit( springai::Unit* u, struct UnitInformationContainer c );
	int CalculateDPS( springai::WeaponDef* w );

};
}
#endif
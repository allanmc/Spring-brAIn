#ifndef _BRAINSPACE_THREATMAP_H
#define _BRAINSPACE_THREATMAP_H


#include "types.h"
#include "BrainMap.h"

#include "WeaponMount.h"
#include "WeaponDef.h"
#include "Damage.h"

namespace brainSpace {
///A map keeping information the estimated threat at a given cell.
/**
 * The threat is calculated from enemies last know position, and the the value is in total DPS.
 */
class ThreatMap : public BrainMap
{
public:
	ThreatMap( AIClasses* aiClasses);
	virtual ~ThreatMap();
	void Update();

	float GetThreatAtPos( SAIFloat3 pos, int armorType = 11 );
private:

	int ArmorType;
	void EffectCell(int index, float value);
	void InsertUnit( springai::Unit* u, struct UnitInformationContainer c );
	int CalculateDPS( springai::WeaponDef* w );

};
}
#endif
#include "ArmyInfo.h"


using namespace brainSpace;
using namespace springai;

ArmyInfo::ArmyInfo( AIClasses* aiClasses )
{
	ai = aiClasses;
	
}

ArmyInfo::~ArmyInfo()
{
}

void ArmyInfo::AddUnit(Unit* unit)
{
	positions[unit->GetUnitId()] = unit->GetPos();

	//Add unit to quadtree, using pos

	unitCount++;
}

void ArmyInfo::RemoveUnit(Unit* unit)
{
	if (unitCount==0)
	{
		return;
	}
	SAIFloat3 pos = positions[unit->GetUnitId()];
	positions.erase(unit->GetUnitId());

	//remove unit from quadtree, using pos

	unitCount--;
}

void ArmyInfo::UpdateUnit(Unit* unit)
{
	SAIFloat3 old_pos = positions[unit->GetUnitId()];
	SAIFloat3 new_pos = unit->GetPos();

	//Update quadtree, using old_pos and new_pos
}
#include "MilitaryUnitGroup.h"

using namespace std;
using namespace springai;
using namespace brainSpace;

MilitaryUnitGroup::MilitaryUnitGroup( AIClasses* aiClasses ):BrainGroup(aiClasses)
{
	status = Regrouping;
}

MilitaryUnitGroup::~MilitaryUnitGroup()
{
}

void MilitaryUnitGroup::SetStatus(Status s)
{
	this->status = s;
}

MilitaryUnitGroup::Status MilitaryUnitGroup::GetStatus()
{
	return status;
}

void MilitaryUnitGroup::Attack(int enemy)
{
	SAttackUnitCommand com;
	com.toAttackUnitId = enemy;
	com.timeOut = 100000000;
	com.options = 0;

	for ( map<Unit*, bool>::iterator it = Units.begin() ; it != Units.end() ; it++ )
	{
		com.unitId = it->first->GetUnitId();
		ai->callback->GetEngine()->HandleCommand(0, -1, COMMAND_UNIT_ATTACK, &com);
	}
}

void MilitaryUnitGroup::Scout(SAIFloat3 pos)
{
	SMoveUnitCommand com;
	com.toPos = pos;
	com.timeOut = 100000000;
	com.options = 0;

	for ( map<Unit*, bool>::iterator it = Units.begin() ; it != Units.end() ; it++ )
	{
		com.unitId = it->first->GetUnitId();
		ai->callback->GetEngine()->HandleCommand(0, -1, COMMAND_UNIT_MOVE, &com);
	}
}

void MilitaryUnitGroup::UnitIdle(springai::Unit *unit)
{
	for ( map<Unit*, bool>::iterator it = Units.begin() ; it != Units.end() ; it++ )
	{
		if(it->first->GetUnitId() == unit->GetUnitId())
		{
			SetStatus(MilitaryUnitGroup::Idle);
			break;
		}
	}
}

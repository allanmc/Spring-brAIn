#include "MilitaryUnitGroup.h"

MilitaryUnitGroup::MilitaryUnitGroup( AICallback* callback ):BrainGroup(callback)
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

	for(int i = 0; i < Units.size(); i++)
	{
		com.unitId = Units[i]->GetUnitId();
		Callback->GetEngine()->HandleCommand(0, -1, COMMAND_UNIT_ATTACK, &com);
	}
}

void MilitaryUnitGroup::Scout(SAIFloat3 pos)
{
	SMoveUnitCommand com;
	com.toPos = pos;
	com.timeOut = 100000000;
	com.options = 0;

	for(int i = 0; i < Units.size(); i++)
	{
		com.unitId = Units[i]->GetUnitId();
		Callback->GetEngine()->HandleCommand(0, -1, COMMAND_UNIT_MOVE, &com);
	}
}

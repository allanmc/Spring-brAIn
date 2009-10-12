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

///@see Status
void MilitaryUnitGroup::SetStatus(Status s)
{
	this->status = s;
}

///@see Status
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
		ai->callback->GetEngine()->HandleCommand(0, -1, COMMAND_UNIT_ATTACK, &com);
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
		ai->callback->GetEngine()->HandleCommand(0, -1, COMMAND_UNIT_MOVE, &com);
	}
}

void MilitaryUnitGroup::UnitIdle(springai::Unit *unit)
{
	for(int j = 0; j < Units.size(); j++)
		{
			if(Units[j]->GetUnitId() == unit->GetUnitId() && Units.size() > 9)
			{
				SetStatus(MilitaryUnitGroup::Idle);
				break;
			}
		}
}

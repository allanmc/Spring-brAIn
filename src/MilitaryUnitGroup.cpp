#include "MilitaryUnitGroup.h"

using namespace std;
using namespace springai;
using namespace brainSpace;

MilitaryUnitGroup::MilitaryUnitGroup( AIClasses* aiClasses, int groupID ):BrainGroup(aiClasses, groupID)
{
	Status = MILI_UNIT_GRP_REGROUPING;
}

MilitaryUnitGroup::~MilitaryUnitGroup()
{
}

void MilitaryUnitGroup::Guard(int unit)
{

	SGuardUnitCommand g;
	g.toGuardUnitId = unit;
	g.timeOut = 999999;

	for ( map<Unit*, bool>::iterator it = Units.begin() ; it != Units.end() ; it++ )
	{
		g.unitId = it->first->GetUnitId();
		ai->callback->GetEngine()->HandleCommand(0, -1, COMMAND_UNIT_GUARD, &g);
	}
	
}

///@see Status
void MilitaryUnitGroup::SetStatus(MilitaryGroupStatus s)
{
	Status = s;
}

///@see Status
MilitaryUnitGroup::MilitaryGroupStatus MilitaryUnitGroup::GetStatus()
{
	return Status;
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
		it->second = false;
	}
}

bool MilitaryUnitGroup::IsIdle()
{
	for ( map<Unit*, bool>::iterator it = Units.begin() ; it != Units.end() ; it++ )
	{
		if ( it->second == false )
			return false;
	}
	return true;
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
		it->second = false;
	}
	
}

void MilitaryUnitGroup::UnitIdle(springai::Unit *unit)
{
	for ( map<Unit*, bool>::iterator it = Units.begin() ; it != Units.end() ; it++ )
	{
		if(it->first->GetUnitId() == unit->GetUnitId())
		{
			it->second = true;
			//Check if all units in group are now idle
			ai->utility->Log( ALL, GROUPING, "");
			ai->utility->Log( ALL, GROUPING, "Unit %d has gone idle", unit->GetUnitId() );
			ai->utility->Log( ALL, GROUPING, "GroupID %d", GroupID );
			ai->utility->Log( ALL, GROUPING,"Status %d",Status );
			bool allUnitsIdle = true;
			if ( !(Status == MILI_UNIT_GRP_REGROUPING ))
			{
				ai->utility->Log( ALL, GROUPING, "Group %d is not regrouping", GroupID );
				for ( map<Unit*, bool>::iterator it2 = Units.begin() ; it2 != Units.end() ; it2++ )
				{
					if ( it2->second == false )
					{
						ai->utility->Log( ALL, GROUPING, "Unit %d is not idle", it2->first->GetUnitId() );
						allUnitsIdle = false;
						break;
					}
				}
				if ( allUnitsIdle )
				{
					ai->utility->Log( ALL, GROUPING, "All units were idle in group %d!", GroupID );
					SetStatus( MILI_UNIT_GRP_IDLE );
				}
				break;
			}
		}
	}
}

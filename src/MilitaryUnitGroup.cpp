#include "MilitaryUnitGroup.h"
#include "CurrentCommand.h"

using namespace std;
using namespace springai;
using namespace brainSpace;

MilitaryUnitGroup::MilitaryUnitGroup( AIClasses* aiClasses, int groupID, bool airGroup ):BrainGroup(aiClasses, groupID)
{
	Status = MILI_UNIT_GRP_IDLE;
	UnitTypes.clear();
	AirGroup = airGroup;
}

MilitaryUnitGroup::~MilitaryUnitGroup()
{
}

void MilitaryUnitGroup::Guard(int unit)
{

	SGuardUnitCommand g;
	g.toGuardUnitId = unit;
	g.timeOut = 999999;

	for ( map<int, bool>::iterator it = Units.begin() ; it != Units.end() ; it++ )
	{
		g.unitId = it->first;
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
	com.timeOut = 100000000;
	com.options = 0;
	com.toAttackUnitId = enemy;

	for ( map<int, bool>::iterator it = Units.begin() ; it != Units.end() ; it++ )
	{
		com.unitId = it->first;
		ai->callback->GetEngine()->HandleCommand(0, -1, COMMAND_UNIT_ATTACK, &com);
		it->second = false;
	}
	//ai->utility->ChatMsg("Attack order issued");
	Status = MILI_UNIT_GRP_ATTACKING;
}

void MilitaryUnitGroup::Attack(vector<int> enemies)
{
	SAttackUnitCommand com;
	com.timeOut = 100000000;
	com.options = UNIT_COMMAND_OPTION_SHIFT_KEY;

	for ( map<int, bool>::iterator it = Units.begin() ; it != Units.end() ; it++ )
	{
		com.unitId = it->first;
		for ( unsigned int i = 0 ; i < enemies.size() ; i++ )
		{
			com.toAttackUnitId = enemies[i];
			ai->callback->GetEngine()->HandleCommand(0, -1, COMMAND_UNIT_ATTACK, &com);
		}
		it->second = false;
	}
	Status = MILI_UNIT_GRP_ATTACKING;
}

bool MilitaryUnitGroup::IsIdle()
{
	ai->utility->Log(ALL,MISC, "isidle: %d", Units.size());
	for ( map<int, bool>::iterator it = Units.begin() ; it != Units.end() ; it++ )
	{			
		//if ( it->second == false )
		Unit* u = Unit::GetInstance(ai->callback, it->first );
		if(u->GetCurrentCommands().size() > 0)
		{
			ai->utility->Log(ALL,MISC, "unit %d, size of commands: %d", it->first, u->GetCurrentCommands().size());
			for(unsigned int i = 0; i < u->GetCurrentCommands().size(); i++)
			{
				ai->utility->Log(ALL,MISC, "command: %d", u->GetCurrentCommands()[i]->GetCommandId());
			}
			return false;
		}
		delete u;
	}
	return true;
}

void MilitaryUnitGroup::Scout(SAIFloat3 pos)
{
	SMoveUnitCommand com;
	com.toPos = pos;
	com.timeOut = 100000000;
	com.options = UNIT_COMMAND_OPTION_SHIFT_KEY;

	for ( map<int, bool>::iterator it = Units.begin() ; it != Units.end() ; it++ )
	{
		Unit* u = Unit::GetInstance(ai->callback, it->first );
		com.unitId = it->first;
		Engine* e = ai->callback->GetEngine();
		e->HandleCommand(0, -1, COMMAND_UNIT_MOVE, &com);
		it->second = false;
		delete u;
		u = NULL;
		delete e;
		e = NULL;
	}
}

bool MilitaryUnitGroup::UnitIdle(springai::Unit *unit)
{
	for ( map<int, bool>::iterator it = Units.begin() ; it != Units.end() ; it++ )
	{
		if(it->first == unit->GetUnitId())
		{
			it->second = true;
			//Check if all units in group are now idle
			bool allUnitsIdle = true;
			if ( !(Status == MILI_UNIT_GRP_REGROUPING ))
			{
				ai->utility->Log( ALL, GROUPING, "Group %d is not regrouping", GroupID );
				for ( map<int, bool>::iterator it2 = Units.begin() ; it2 != Units.end() ; it2++ )
				{
					if ( it2->second == false )
					{
						ai->utility->Log( ALL, GROUPING, "Unit %d is not idle", it2->first );
						allUnitsIdle = false;
						return false;
					}
				}
				if ( allUnitsIdle )
				{
					//ai->utility->ChatMsg("MiliGroup: All units idle");
					//ai->utility->Log( ALL, GROUPING, "All units were idle in group %d!", GroupID );
					SetStatus( MILI_UNIT_GRP_IDLE );
					return true;
				}
				break;
			}
		}
	}
	return false;
}


bool MilitaryUnitGroup::AddUnit(springai::Unit *unit)
{
	bool canInsert = true;
	//if the unit already is in the group, or the group has the maximum size, return
	if ( Units.size() >= MILI_GROUP_MAXSIZE || (Units.find( unit->GetUnitId()) != Units.end() ) )
	{
		return false;
	}
	//See if the 3-type constraint will be obeyed
	UnitDef* d = unit->GetDef();

	int type = d->GetUnitDefId();
	if ( AirGroup )
	{
		if ( !d->IsAbleToFly() )
			canInsert = false;
	}
	else
	{
		if ( d->IsAbleToFly() )
			canInsert = false;
	}
	
	delete d;
	if ( canInsert )
	{
		//Okay so the unit can fly and is put into an airgroup, or the unit cannot fly and is put into a groundgroup
		canInsert = false; //reset flag and see if there is room for this unit in the group
		if ( UnitTypes.find( type ) == UnitTypes.end() )
		{
			if ( UnitTypes.size() < MILI_GROUP_MAXTYPES )
			{
				//the type was not found, but we can still insert the unit since the number of types is at most 2
				canInsert = true;
				UnitTypes[type] = 0;
			}
		}
		else
		{
			//We already have this type in the group
			canInsert = true;
		}


		if ( canInsert )
		{
			Units[unit->GetUnitId()] = true;
			UnitTypes[type] += 1;
			
			SSetFireStateUnitCommand s;
			s.unitId = unit->GetUnitId();
			s.timeOut = 100000;
			s.fireState = 0;
			Engine* e = ai->callback->GetEngine();
			e->HandleCommand( 0, -1, COMMAND_UNIT_SET_FIRE_STATE, &s );
			delete e;
		}
	}
	return canInsert;
}

map<int, int> MilitaryUnitGroup::GetUnitTypes()
{
	return UnitTypes;
}

bool MilitaryUnitGroup::IsAirGroup()
{
	return AirGroup;
}

void MilitaryUnitGroup::StopGroup()
{	
	SStopUnitCommand com;
	com.options = 0;

	for ( map<int, bool>::iterator it = Units.begin() ; it != Units.end() ; it++ )
	{
		com.unitId = it->first;
		ai->callback->GetEngine()->HandleCommand(0, -1, COMMAND_UNIT_STOP, &com);
		it->second = true;
	}
}

void MilitaryUnitGroup::FireAtWill()
{
	SSetFireStateUnitCommand com;
	com.options = 0;
	com.fireState = 2;

	for ( map<int, bool>::iterator it = Units.begin() ; it != Units.end() ; it++ )
	{
		com.unitId = it->first;
		ai->callback->GetEngine()->HandleCommand(0, -1, COMMAND_UNIT_SET_FIRE_STATE, &com);
		it->second = true;
	}
}
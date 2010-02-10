#include "RL_State.h"
#include "RL.h"

using namespace brainSpace;

RL_State::RL_State()
{
}

RL_State::RL_State( AIClasses* aiClasses, int node, unsigned short int type)
{
	Node = node;
	ai = aiClasses;
	switch (node)
	{
	case 0: //Flat old
		{
			terminal = false;
			int labCount = ai->knowledge->selfInfo->baseInfo->CountBuildingsByName("armlab");
			ai->utility->Log(ALL, MISC, "labCount: %d", labCount);
			int solarCount = ai->knowledge->selfInfo->baseInfo->CountBuildingsByName("armsolar");
			ai->utility->Log(ALL, MISC, "solarCount: %d", solarCount);
			int mexCount = ai->knowledge->selfInfo->baseInfo->CountBuildingsByName("armmex");
			ai->utility->Log(ALL, MISC, "mexCount: %d", mexCount);
			int rockoCount = 0;
			int commanderDead = 0;
			if (NEWSCENARIO) {
				rockoCount = ai->knowledge->selfInfo->armyInfo->CountUnitsByName("armrock");
				ai->utility->Log(ALL, MISC, "rockoCount: %d", rockoCount);
				commanderDead = ai->commanderDead;
				ai->utility->Log(ALL, MISC, "commanderDead: %d", commanderDead);
				if (rockoCount >= RL_ROCKO_INDEX-1)
				{
					rockoCount = RL_ROCKO_INDEX-1;
				}
				if (commanderDead != 0)
				{
					terminal = true;
				}
				ID = commanderDead*RL_MEX_INDEX*RL_SOLAR_INDEX*RL_LAB_INDEX*RL_ROCKO_INDEX + rockoCount*RL_MEX_INDEX*RL_SOLAR_INDEX*RL_LAB_INDEX + labCount*RL_MEX_INDEX*RL_SOLAR_INDEX + solarCount*RL_MEX_INDEX + mexCount;
			} else {
				if (labCount==4) {
					terminal = true;
				}
				ID = labCount*RL_MEX_INDEX*RL_SOLAR_INDEX + solarCount*RL_MEX_INDEX + mexCount;
			}
			if(labCount < RL_LAB_INDEX-1)
				Actions.push_back(RL_Action(ai->utility->GetUnitDef("armlab")->GetUnitDefId(),0,false));
			if(solarCount < RL_SOLAR_INDEX-1)
				Actions.push_back(RL_Action(ai->utility->GetUnitDef("armsolar")->GetUnitDefId(),1,false));
			if(mexCount < RL_MEX_INDEX-1)
				Actions.push_back(RL_Action(ai->utility->GetUnitDef("armmex")->GetUnitDefId(),2,false));
			if (NEWSCENARIO) {
				if (labCount > 0 && rockoCount < RL_ROCKO_INDEX-1)
				{
					Actions.push_back(RL_Action(ai->utility->GetUnitDef("armrock")->GetUnitDefId(),3,false));
				}
				if (rockoCount > 0)
				{
					Actions.push_back(RL_Action(RL_ATTACK_ACTION, 4, false));
				}
			}
		}break;
	default://error
		Node = -1;
	}
}

RL_State::~RL_State()
{
	//Actions.clear();
}

int RL_State::GetID()
{
	return ID;
}

vector<RL_Action> RL_State::GetActions()
{
	ai->utility->Log(ALL, MISC, "RL_State::GetActions.size = %i", Actions.size());
	return Actions;
}

void RL_State::DeleteAction(int actionID)
{
	for (int i = 0; i < (int)Actions.size(); i++)
	{
		if(Actions[i].ID == actionID)
		{
			//delete Actions[i];
			//Actions[i] = NULL;
			Actions.erase(Actions.begin()+i);
			return;
		}
	}
}

bool RL_State::IsTerminal()
{
	return terminal;
}

int RL_State::GetNode()
{
	return Node;
}

bool RL_State::operator==(const RL_State &other) const
{
	return (ID == other.ID);
}

RL_State & RL_State::operator=(const RL_State &rhs)
{
	if (this == &rhs) // Same object?
      return *this;// Yes, so skip assignment, and just return *this.

	Actions = rhs.Actions;
	ID = rhs.ID;
	Node = rhs.Node;
	terminal = rhs.terminal;
	ai = rhs.ai;
	return *this;
}
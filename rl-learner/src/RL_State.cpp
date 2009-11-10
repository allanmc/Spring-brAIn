/*
#include "RL_State.h"
#include "RL.h"

using namespace brainSpace;

RL_State::RL_State()
{
}

RL_State::RL_State( AIClasses* aiClasses, int node)
{
	Node = node;
	ai = aiClasses;
	switch(node)
	{
	case 0://root
		{
			int labCount = ai->knowledge->selfInfo->baseInfo->CountBuildingsByName("armlab");
			int solarCount = ai->knowledge->selfInfo->baseInfo->CountBuildingsByName("armsolar");
			int mexCount = ai->knowledge->selfInfo->baseInfo->CountBuildingsByName("armmex");
			terminal = (labCount==4);
			ID = labCount*RL_MEX_INDEX*RL_SOLAR_INDEX + solarCount*RL_MEX_INDEX + mexCount;
			if(labCount < 4)
				Actions.push_back(RL_Action(ai->utility->GetUnitDef("armlab")->GetUnitDefId(),0,false));
			if(solarCount < 19)
				Actions.push_back(RL_Action(ai->utility->GetUnitDef("armsolar")->GetUnitDefId(),1,false));
			if(mexCount < 19)
				Actions.push_back(RL_Action(ai->utility->GetUnitDef("armmex")->GetUnitDefId(),2,false));
			
			
		}break;
	default://error
		Node = -1;
	}
}

RL_State::~RL_State()
{
	Actions.clear();
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

*/
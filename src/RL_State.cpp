#include "RL_State.h"
#include "RL.h"

using namespace brainSpace;

RL_State::RL_State()
{
}

RL_State::RL_State( AIClasses* ai, vector<RL_Action> actions, bool terminal )
{
	Actions = actions;
	this->terminal = terminal;
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
			int EnoughLabs = (labCount >= 4);
			terminal = 	(EnoughLabs ? true : false);
			int affordable = ai->knowledge->selfInfo->resourceInfo->IsAffordableToBuild(ai->utility->GetUnitDef("armcom"),
				ai->utility->GetUnitDef("armlab"));
			bool CanBuildLab = (affordable == 0);
			
			ID = (CanBuildLab ? 2 : 0) + (EnoughLabs ? 1 : 0);
			ai->utility->Log(ALL, LOG_RL, "RL_State, affordable = %i", affordable);
			ai->utility->Log(ALL, LOG_RL, "RL_State, CanBuildLab = %i", CanBuildLab);
			ai->utility->Log(ALL, LOG_RL, "RL_State, EnoughLabs = %i", EnoughLabs);
			ai->utility->Log(ALL, LOG_RL, "RL_State, ID = %i", ID);
			Actions.push_back(RL_Action(1,0,true));
			Actions.push_back(RL_Action(2,1,true));
		}break;
	case 1://factory
		{
			terminal = true;
			int LabCount = ai->knowledge->selfInfo->baseInfo->CountBuildingsByName("armlab");
			int PlantCount = ai->knowledge->selfInfo->baseInfo->CountBuildingsByName("armvp");
			ID = PlantCount * 5 + LabCount;
			if(PlantCount < 4)
				Actions.push_back(RL_Action(ai->utility->GetUnitDef("armvp")->GetUnitDefId(),0,false));
			if(LabCount < 4)
				Actions.push_back(RL_Action(ai->utility->GetUnitDef("armlab")->GetUnitDefId(),1,false));
		}break;
	case 2://resource
		{
			terminal = true;
			int MexCount = ai->knowledge->selfInfo->baseInfo->CountBuildingsByName("armmex");
			int SolarCount = ai->knowledge->selfInfo->baseInfo->CountBuildingsByName("armsolar");
			ID = MexCount * 20 + SolarCount;
			if(MexCount < 19)
				Actions.push_back(RL_Action(ai->utility->GetUnitDef("armmex")->GetUnitDefId(),0,false));
			if(SolarCount < 19)
				Actions.push_back(RL_Action(ai->utility->GetUnitDef("armsolar")->GetUnitDefId(),1,false));
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
#include "RL_State.h"
#include "RL.h"

using namespace brainSpace;

RL_State::RL_State()
{
}

RL_State::RL_State(Game *g, int node, unsigned short int type)
{
	game = g;
	Node = node;
	switch (type)
	{
	case 0: //Flat old
		{
			int labCount = game->buildings[RL_LAB_ID];
			int solarCount = game->buildings[RL_SOLAR_ID];
			int mexCount = game->buildings[RL_MEX_ID];
			terminal = (labCount==4);
			ID = labCount*RL_MEX_INDEX*RL_SOLAR_INDEX + solarCount*RL_MEX_INDEX + mexCount;
			if(labCount < 4)
				Actions.push_back(RL_Action(RL_LAB_ID,0,false));
			if(solarCount < 19)
				Actions.push_back(RL_Action(RL_SOLAR_ID,1,false));
			if(mexCount < 19)
				Actions.push_back(RL_Action(RL_MEX_ID,2,false));	
		}break;
	case 1: //H old
		switch(node)
		{
		case 0://root
			{
				int labCount = game->buildings[RL_LAB_ID];
				int EnoughLabs = (labCount >= 4);
				terminal = 	(EnoughLabs ? true : false);
				int affordable = game->CanBuild(RL_LAB_ID);
				bool CanBuildLab = (affordable == 0);
				
				ID = (CanBuildLab ? 2 : 0) + (EnoughLabs ? 1 : 0);
				Actions.push_back(RL_Action(1,0,true));
				Actions.push_back(RL_Action(2,1,true));
			}break;
		case 1://factory
			{
				terminal = true;
				int LabCount = game->buildings[RL_LAB_ID];
				int PlantCount = game->buildings[RL_PLANT_ID];
				ID = PlantCount * 5 + LabCount;
				if(PlantCount < 4)
					Actions.push_back(RL_Action(RL_PLANT_ID,0,false));
				if(LabCount < 4)
					Actions.push_back(RL_Action(RL_LAB_ID,1,false));
			}break;
		case 2://resource
			{
				terminal = true;
				int MexCount = game->buildings[RL_MEX_ID];
				int SolarCount = game->buildings[RL_SOLAR_ID];
				ID = MexCount * 20 + SolarCount;
				if(MexCount < 19)
					Actions.push_back(RL_Action(RL_MEX_ID,0,false));
				if(SolarCount < 19)
					Actions.push_back(RL_Action(RL_SOLAR_ID,1,false));
			}break;
		default://error
			Node = -1;
		}
		break;
	case 2: //Resource Needs
		
		break;
	default:
		break;
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
	return *this;
}
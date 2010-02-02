#include "RL_State.h"
#include "RL.h"

using namespace brainSpace;

RL_State::RL_State()
{
}

RL_State::RL_State(Game *g, int node, unsigned short int type, int buildingToBuild)
{
	if (type!=2) {
		RL_State(g, node, type);
		return;
	}
	game = g;
	Node = node;

	if (node == -1)
	{
		ID = -1;
		return;
	}

	terminal = false;
	short unsigned int mNeeds = game->GetDiscreteResource(game->BuildingCosts(RL_MEX_ID, buildingToBuild));
	short unsigned int eNeeds = game->GetDiscreteResource(game->BuildingCosts(RL_SOLAR_ID, buildingToBuild));
	short unsigned int mAvailable = game->GetDiscreteResource(game->GetAvailableResources(RL_MEX_ID, game->GetBuildTime(buildingToBuild)));
	short unsigned int eAvailable = game->GetDiscreteResource(game->GetAvailableResources(RL_SOLAR_ID, game->GetBuildTime(buildingToBuild)));
	
	//cout << "State: " << mNeeds << " - " << eNeeds << " - " << mAvailable << " - " << eAvailable << "\n";
	
	float dStates = DISCRETE_STATES;
	ID =  mNeeds*(int)pow(dStates,3) + eNeeds*(int)pow(dStates,2) + mAvailable*(int)pow(dStates,1) + eAvailable;
	//Actions.push_back(RL_Action(-1,0,false));
	//Actions.push_back(RL_Action(RL_SOLAR_ID,1,false));
	//Actions.push_back(RL_Action(RL_MEX_ID,2,false));
	//Actions.push_back(RL_Action(RL_ROCKO_ID,3,false));
}

RL_State::RL_State(Game *g, int node, unsigned short int type)
{
	game = g;
	Node = node;


	if (node == -1)
	{
		ID = -1;
		return;
	}


	switch (type)
	{
	case 0: //Flat old
		{
			terminal = false;
			int labCount = game->units[RL_LAB_ID];
			int solarCount = game->units[RL_SOLAR_ID];
			int mexCount = game->units[RL_MEX_ID];
			int rockoCount = game->units[RL_ROCKO_ID];
			/*
			if (rockoCount >= RL_ROCKO_INDEX-1)
			{
				terminal = true;
				rockoCount = RL_ROCKO_INDEX-1;
			}
			*/
			if (labCount >= RL_LAB_INDEX-1)
			{
				terminal = true;
				labCount = RL_LAB_INDEX-1;
			}

			ID = /*rockoCount*RL_MEX_INDEX*RL_SOLAR_INDEX*RL_LAB_INDEX +*/ labCount*RL_MEX_INDEX*RL_SOLAR_INDEX + solarCount*RL_MEX_INDEX + mexCount;
			if(labCount < RL_LAB_INDEX-1)
				Actions.push_back(RL_Action(RL_LAB_ID,0,false));
			if(solarCount < RL_SOLAR_INDEX-1)
				Actions.push_back(RL_Action(RL_SOLAR_ID,1,false));
			if(mexCount < RL_MEX_INDEX-1)
				Actions.push_back(RL_Action(RL_MEX_ID,2,false));
			/*
			if (labCount > 0 && rockoCount < RL_ROCKO_INDEX)
			{
				Actions.push_back(RL_Action(RL_ROCKO_ID,3,false));
			}
			*/

		}break;
	case 1: //H old
		switch(node)
		{
		case 0://root
			{
				int labCount = game->units[RL_LAB_ID];
				int EnoughLabs = (labCount >= 4);
				terminal = 	(EnoughLabs ? true : false);
				int affordable = game->CanBuild(RL_LAB_ID, 1);
				bool CanBuildLab = (affordable == 0);
				
				ID = (CanBuildLab ? 2 : 0) + (EnoughLabs ? 1 : 0);
				Actions.push_back(RL_Action(1,0,true));
				Actions.push_back(RL_Action(2,1,true));
			}break;
		case 1://factory
			{
				terminal = true;
				int LabCount = game->units[RL_LAB_ID];
				int RockoCount = game->units[RL_ROCKO_ID];
				ID = RockoCount * 5 + LabCount;
				if(RockoCount < 4)
					Actions.push_back(RL_Action(RL_ROCKO_ID,0,false));
				if(LabCount < 4)
					Actions.push_back(RL_Action(RL_LAB_ID,1,false));
			}break;
		case 2://resource
			{
				terminal = true;
				int MexCount = game->units[RL_MEX_ID];
				int SolarCount = game->units[RL_SOLAR_ID];
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
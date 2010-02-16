#include "RL_State.h"
#include "RL.h"

using namespace brainSpace;

RL_State::RL_State()
{
	ID = -1;
}

RL_State::RL_State(Game *g, unsigned short int type)
{
	game = g;

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
				Actions.push_back(RL_Action(RL_LAB_ID,0));
			if(solarCount < RL_SOLAR_INDEX-1)
				Actions.push_back(RL_Action(RL_SOLAR_ID,1));
			if(mexCount < RL_MEX_INDEX-1)
				Actions.push_back(RL_Action(RL_MEX_ID,2));
			/*
			if (labCount > 0 && rockoCount < RL_ROCKO_INDEX)
			{
				Actions.push_back(RL_Action(RL_ROCKO_ID,3));
			}
			*/

		}break;
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
	terminal = rhs.terminal;
	return *this;
}
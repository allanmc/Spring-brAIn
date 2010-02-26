#include "RL_State.h"
#include "RL.h"

using namespace brainSpace;

RL_State::RL_State()
{
	ID = -1;
}

RL_State::RL_State(Game *g, int agentId)
{
	//TODO: use agentId to find out what the other agent is doing right now
	game = g;
	int solarCount = game->units[SOLAR_ID];
	int mexCount = game->units[MEX_ID];

	switch (RL_TYPE)
	{
	case 0: //Flat old
		{
			terminal = false;
			int labCount = game->units[LAB_ID];

			//int rockoCount = game->units[ROCKO_ID];

			if (labCount >= RL_LAB_INDEX-1)
			{
				terminal = true;
				labCount = RL_LAB_INDEX-1;
			}

			ID = labCount*RL_MEX_INDEX*RL_SOLAR_INDEX + solarCount*RL_MEX_INDEX + mexCount;
			if(labCount < RL_LAB_INDEX-1)
				Actions.push_back(RL_Action(LAB_ID,0));
			if(solarCount < RL_SOLAR_INDEX-1)
				Actions.push_back(RL_Action(SOLAR_ID,1));
			if(mexCount < RL_MEX_INDEX-1)
				Actions.push_back(RL_Action(MEX_ID,2));

		}break;
	case 1://two builders
		{
			terminal = false;
			//storage
			double metalStore = game->GetAvailableResources(MEX_ID,0.0f);
			double energyStore = game->GetAvailableResources(SOLAR_ID,0.0f);
			//income
			double metalProduction = game->GetTotalProduction(MEX_ID);
			double energyProduction = game->GetTotalProduction(SOLAR_ID);
			//other agents work
			int concurrent = game->UnitBeingBuildByBuilder((agentId == 0? 1: 0));//only support for two
			//number of labs
			int labCount = game->units[LAB_ID];
			if (labCount >= RL_LAB_INDEX-1)
			{
				terminal = true;
				labCount = RL_LAB_INDEX-1;
			}

			//set ID
			ID = concurrent + 1;// concurrent ranges from -1 to max id (-1 for no building)
			ID = ID*4 + (metalStore > 150 ? (metalStore > 600 ? 3 : 2) : (metalStore > 50 ? 1 : 0) );
			ID = ID*2 + (energyStore > 500 ? 1 : 0 );
			ID = ID*4 + (metalProduction > 5 ? (metalProduction > 8 ? 3 : 2) : (metalProduction > 2.5f ? 1 : 0) );
			ID = ID*3 + (energyProduction > 26 ? 2 : (energyProduction > 15 ? 1 : 0) );
			ID = ID*RL_LAB_INDEX + labCount;

			//set actions available
			Actions.push_back(RL_Action(LAB_ID,0));
			if(solarCount < RL_SOLAR_INDEX-1)
				Actions.push_back(RL_Action(SOLAR_ID,1));
			if(mexCount < RL_MEX_INDEX-1)
				Actions.push_back(RL_Action(MEX_ID,2));
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
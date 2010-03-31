#include "RL_State.h"
#include "RL.h"

using namespace brainSpace;

int RL_State::lastLabCount;//static from .h file

RL_State::RL_State()
{
	ID = -1;
}

RL_State::RL_State(Game *g, int agentId)
{
	//TODO: use agentId to find out what the other agent is doing right now
	game = g;
	//int solarCount = game->units[SOLAR_ID];
	//int mexCount = game->units[MEX_ID];

	switch (RL_TYPE)
	{
	case 0://continueos construction
		{
			terminal = false;
			int labCount = game->units[LAB_ID];
			//storage
			double metalStore = game->GetAvailableResources(MEX_ID,0.0f);
			double energyStore = game->GetAvailableResources(SOLAR_ID,0.0f);
			//income
			double metalProduction = game->GetTotalProduction(MEX_ID) - labCount*5;
			double energyProduction = game->GetTotalProduction(SOLAR_ID) - game->GetResourceUsage(SOLAR_ID) - labCount*50;
			//other agents work
			
			int concurrent = 0;
			int value = 0;
			int time_remaining = 0;

			isBuildingLab = false;
			for ( int i = 0; i < NUM_LEARNERS ; i++ )
			{
				if ( i == agentId )
				{
					continue;
				}
				
				value = game->UnitBeingBuildByBuilder(i) + 1; // [0;3]
				if ( value > 0 ) 
				{
					if(value-1 == LAB_ID)
					{
						isBuildingLab = true;
						cerr << "pik" << endl;
						exit(0);
					}
					//time_remaining = min(game->GetPercentRemaining(i) / (100/5), 4); // [0;4]
					//value = (value-1)*5 + time_remaining + 1;
				}
				concurrent = concurrent*4 + value;
			}
			//number of labs
			#ifdef USE_NEW_REWARD_CODE
			if(labCount > RL_LAB_INDEX-1)
			{
				//this->lastLabCount = labCount; //Moved to main.
				terminal = true;
			}
#else
			if(labCount > lastLabCount)
			{
				//this->lastLabCount = labCount; //Moved to main.
				terminal = true;
			}
#endif
			//set ID --- Update RL::RL() when changing the ID calculation
			ID = concurrent;
			ID = ID*4 + (metalStore > 600 ? (metalStore > 900 ? 3 : 2) : (metalStore > 300 ? 1 : 0) );
			ID = ID*4 + (energyStore > 600 ? (energyStore > 900 ? 3 : 2) : (energyStore > 300 ? 1 : 0) );
			ID = ID*4 + (metalProduction > REWARD_METAL_MAX ? (metalProduction > REWARD_METAL_MAX+5 ? 3 : 2) : (metalProduction > 0 ? 1 : 0) );
			ID = ID*4 + (energyProduction > REWARD_ENERGY_MAX ? (energyProduction > REWARD_ENERGY_MAX+50 ? 3 : 2) : (energyProduction > 0 ? 1 : 0) );

			//set actions available
			Actions.push_back(RL_Action(LAB_ID,0));
			Actions.push_back(RL_Action(SOLAR_ID,1));
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

//Update This when changing the ID calculation
/*
bool RL_State::isBuildingLab()
{
	bool buildingLab = false;
	int conCur = ID / (4*4*4*4);
	for (int i = 0; i<NUM_LEARNERS-1; i++) {
		int building = (conCur / (16*i)) % 16
		if (building != 0) {
			int id = building / 5;
			if (id == LAB_ID) {
				buildingLab = true;
			}
		}
	}
	return buildingLab;
}*/

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
	isBuildingLab = rhs.isBuildingLab;
	return *this;
}
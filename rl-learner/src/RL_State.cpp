#include "RL_State.h"
#include "RL.h"

using namespace brainSpace;

int RL_State::lastLabCount;//static from .h file

int sumrange(int max, int min, int num){
	return (max + min)*num/2;
}

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
			double metalProduction = game->GetTotalProduction(MEX_ID) - game->GetBuildersUsage(MEX_ID);//- labCount*5;
			double energyProduction = game->GetTotalProduction(SOLAR_ID) - game->GetResourceUsage(SOLAR_ID) - game->GetBuildersUsage(SOLAR_ID);//- labCount*50;
			//other agents work
			
			int concurrent = 0;
			int value = 0;
			int time_remaining = 0;
			int actionCount[CONCURRENT_A];
			for ( int i = 0; i < CONCURRENT_A ; i++ ) {
				actionCount[i] = 0;
			}
			isBuildingLab = false;
			for ( int i = 0; i < NUM_LEARNERS ; i++ )
			{
				if ( i == agentId )
				{
					continue;
				}
				
				value = game->UnitBeingBuildByBuilder(i); // [-1;2]
				if ( value >= 0 ) 
				{
					actionCount[value]++;
					if(value == LAB_ID)
					{
						isBuildingLab = true;
					}
					if (CONCURRENT_SS==2)
					{
						time_remaining = min(game->GetPercentRemaining(i) / (100/CONCURRENT_T), CONCURRENT_T-1); // [0;4]
						value = value*CONCURRENT_T + time_remaining;
					}
				}
				if (CONCURRENT_SS==1)
				{
					concurrent = concurrent*(CONCURRENT_A+1) + (value + 1);
				}
				else if (CONCURRENT_SS==2)
				{
					concurrent = concurrent*(CONCURRENT_A*CONCURRENT_T+1) + (value + 1);
				}
			}

			if ( CONCURRENT_SS==3)
			{
				concurrent = GetConCurId(actionCount); 
				if (concurrent==5) {
					int i = 0;
				}
			}
			else if (CONCURRENT_SS==4 )
			{
				for ( int i = 0; i < CONCURRENT_A ; i++ )
				{
					value = min(actionCount[i], CONCURRENT_I-1);
					concurrent = concurrent*CONCURRENT_I + value;
				}
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
			ID = ID*REWARD_METAL_STORE_STATES	+ GetDiscrete(REWARD_METAL_STORE_MIN, REWARD_METAL_STORE_MAX, REWARD_METAL_STORE_STATES, metalStore); //*4 + (metalStore > 600 ? (metalStore > 900 ? 3 : 2) : (metalStore > 300 ? 1 : 0) );
			ID = ID*REWARD_ENERGY_STORE_STATES	+ GetDiscrete(REWARD_ENERGY_STORE_MIN, REWARD_ENERGY_STORE_MAX, REWARD_ENERGY_STORE_STATES, energyStore); //*4 + (energyStore > 600 ? (energyStore > 900 ? 3 : 2) : (energyStore > 300 ? 1 : 0) );
			ID = ID*REWARD_METAL_STATES			+ GetDiscrete(STATE_METAL_MIN, STATE_METAL_MAX, REWARD_METAL_STATES, metalProduction); //*4 + (metalProduction > REWARD_METAL_MAX ? (metalProduction > REWARD_METAL_MAX+5 ? 3 : 2) : (metalProduction > 0 ? 1 : 0) );
			ID = ID*REWARD_ENERGY_STATES		+ GetDiscrete(STATE_ENERGY_MIN, STATE_ENERGY_MAX, REWARD_ENERGY_STATES, energyProduction); //*4 + (energyProduction > REWARD_ENERGY_MAX ? (energyProduction > REWARD_ENERGY_MAX+50 ? 3 : 2) : (energyProduction > 0 ? 1 : 0) );

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

int RL_State::GetConCurId(int* actionCount)
{
		int id = 0;
		
		assert(CONCURRENT_A==3);

		//action1
		for (int i = 0; i < actionCount[0]; i++) {
			id += sumrange(NUM_LEARNERS - i, 1, NUM_LEARNERS - i);
		}
		//action2
		//id += SumRange(num_units - unit1, num_units - unit2 + 1, unit2-unit1 );
		id += sumrange(NUM_LEARNERS - actionCount[0], NUM_LEARNERS - actionCount[1] + 1, actionCount[1] );
		//action3
		id += actionCount[2];

		return id;
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

short unsigned int RL_State::GetDiscrete(double minv, double maxv, unsigned short int states, double value)
{
	int result = ( (max( minv,min(value, maxv))-minv) / (maxv - minv))*(states-1);
	//if (result < 0 || result > states-1 ) {
	//int crap = 2;
	//}
	return result;
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

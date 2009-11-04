#include "RL_State_Root.h"
#include "global.h"
using namespace brainSpace;

RL_State_Root::RL_State_Root( AIClasses *aiClasses, bool enoughLabs, bool canBuildLab ): RL_State(aiClasses)
{
	EnoughLabs = enoughLabs;
	terminal = 	(EnoughLabs ? true : false);
	CanBuildLab = canBuildLab;
	ID = (CanBuildLab ? 2 : 0) + (EnoughLabs ? 1 : 0);
	Actions.push_back(new RL_Action(1,0,true));//Factory
	Actions.push_back(new RL_Action(2,1,true));//Resource
}

RL_State_Root::RL_State_Root( AIClasses *aiClasses ): RL_State(aiClasses)
{
	int labCount = ai->knowledge->selfInfo->baseInfo->CountBuildingsByName("armlab");
	EnoughLabs = (labCount >= 4);
	terminal = 	(EnoughLabs ? true : false);
	int affordable = ai->knowledge->selfInfo->resourceInfo->IsAffordableToBuild(ai->utility->GetUnitDef("armcom"),
																				ai->utility->GetUnitDef("armlab"));
	CanBuildLab = (affordable == 0);
	ID = (CanBuildLab ? 2 : 0) + (EnoughLabs ? 1 : 0);
	Actions.push_back(new RL_Action(1,0,true));
	Actions.push_back(new RL_Action(2,1,true));
	ai->utility->Log(ALL, LOG_RL, "Action 1 action = %i", Actions[0]->Action);
	ai->utility->Log(ALL, LOG_RL, "Action 2 action = %i", Actions[1]->Action);
}

RL_State_Root::~RL_State_Root()
{
}
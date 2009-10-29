#include "RL_State_Root.h"
#include "global.h"
using namespace brainSpace;

RL_State_Root::RL_State_Root( AIClasses *aiClasses, bool enoughLabs, bool canBuildLab ): RL_State(aiClasses)
{
	EnoughLabs = enoughLabs;
	CanBuildLab = canBuildLab;
	ID = (CanBuildLab ? 2 : 0) + (EnoughLabs ? 1 : 0);
	Actions.push_back(new RL_Action(0,0,true));//Factory
	Actions.push_back(new RL_Action(0,1,true));//Resource
}

RL_State_Root::RL_State_Root( AIClasses *aiClasses ): RL_State(aiClasses)
{
	int labCount = ai->knowledge->selfInfo->baseInfo->CountBuildingsByName("armlab");
	EnoughLabs = (labCount >= 4);
	int affordable = ai->knowledge->selfInfo->resourceInfo->IsAffordableToBuild(ai->utility->GetUnitDef("armcom"),
																				ai->utility->GetUnitDef("armlab"));
	CanBuildLab = (affordable == 0);
	ID = (CanBuildLab ? 2 : 0) + (EnoughLabs ? 1 : 0);
	Actions.push_back(new RL_Action(0,0,true));
	Actions.push_back(new RL_Action(0,1,true));
}

RL_State_Root::~RL_State_Root()
{
}

bool RL_State_Root::IsTerminal()
{
	if(EnoughLabs)
		return true;
	else
		return false;
}
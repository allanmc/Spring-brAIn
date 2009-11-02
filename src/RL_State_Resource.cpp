#include "RL_State_Resource.h"
#include "global.h"
using namespace brainSpace;

RL_State_Resource::RL_State_Resource(AIClasses *aiClasses, int mexCount, int solarCount ): RL_State(aiClasses)
{
	MexCount = mexCount;
	SolarCount = solarCount;
	ID = MexCount * 20 + SolarCount;
	if(MexCount < 19)
		Actions.push_back(new RL_Action(ai->utility->GetUnitDef("armmex")->GetUnitDefId(),0,false));
	if(SolarCount < 19)
		Actions.push_back(new RL_Action(ai->utility->GetUnitDef("armsolar")->GetUnitDefId(),1,false));
}

RL_State_Resource::RL_State_Resource(AIClasses *aiClasses ): RL_State(aiClasses)
{
	MexCount = ai->knowledge->selfInfo->baseInfo->CountBuildingsByName("armmex");
	SolarCount = ai->knowledge->selfInfo->baseInfo->CountBuildingsByName("armsolar");
	ID = MexCount * 20 + SolarCount;
	if(MexCount < 19)
		Actions.push_back(new RL_Action(ai->utility->GetUnitDef("armmex")->GetUnitDefId(),0,false));
	if(SolarCount < 19)
		Actions.push_back(new RL_Action(ai->utility->GetUnitDef("armsolar")->GetUnitDefId(),1,false));
}

RL_State_Resource::~RL_State_Resource()
{
}

bool RL_State_Resource::IsTerminal()
{
	return true; // is this correct?
}
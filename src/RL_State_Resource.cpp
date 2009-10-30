#include "RL_State_Resource.h"
#include "global.h"
using namespace brainSpace;

RL_State_Resource::RL_State_Resource(AIClasses *aiClasses, int mexCount, int solarCount ): RL_State(aiClasses)
{
	MexCount = mexCount;
	SolarCount = solarCount;
	ID = MexCount * RL_MEX_INDEX + SolarCount;
	if(MexCount < RL_MEX_INDEX-1)
		Actions.push_back(new RL_Action(ai->utility->GetUnitDef("armmex")->GetUnitDefId(),0,false));
	if(SolarCount < RL_SOLAR_INDEX-1)
		Actions.push_back(new RL_Action(ai->utility->GetUnitDef("armsolar")->GetUnitDefId(),1,false));
}

RL_State_Resource::RL_State_Resource(AIClasses *aiClasses ): RL_State(aiClasses)
{
	MexCount = ai->knowledge->selfInfo->baseInfo->CountBuildingsByName("armmex");
	SolarCount = ai->knowledge->selfInfo->baseInfo->CountBuildingsByName("armsolar");
	ID = MexCount * RL_MEX_INDEX + SolarCount;
	if(MexCount < RL_MEX_INDEX-1)
		Actions.push_back(new RL_Action(ai->utility->GetUnitDef("armmex")->GetUnitDefId(),0,false));
	if(SolarCount < RL_SOLAR_INDEX-1)
		Actions.push_back(new RL_Action(ai->utility->GetUnitDef("armsolar")->GetUnitDefId(),1,false));
}

RL_State_Resource::~RL_State_Resource()
{
}

bool RL_State_Resource::IsTerminal()
{
	return true; // is this correct?
}
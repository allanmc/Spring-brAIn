#include "RL_State_Factory.h"
#include "global.h"

using namespace brainSpace;

RL_State_Factory::RL_State_Factory(AIClasses *aiClasses, int labCount, int plantCount ): RL_State(aiClasses)
{
	LabCount = labCount;
	PlantCount = plantCount;
	ID = PlantCount * 5 + LabCount;
	if(PlantCount < 4)
		Actions.push_back(new RL_Action(ai->utility->GetUnitDef("armvp")->GetUnitDefId(),0,false));
	if(LabCount < 4)
		Actions.push_back(new RL_Action(ai->utility->GetUnitDef("armlab")->GetUnitDefId(),1,false));
}

RL_State_Factory::RL_State_Factory(AIClasses *aiClasses ): RL_State(aiClasses)
{
	LabCount = ai->knowledge->selfInfo->baseInfo->CountBuildingsByName("armlab");
	PlantCount = ai->knowledge->selfInfo->baseInfo->CountBuildingsByName("armvp");
	ID = PlantCount * 5 + LabCount;
	if(PlantCount < 4)
		Actions.push_back(new RL_Action(ai->utility->GetUnitDef("armvp")->GetUnitDefId(),0,false));
	if(LabCount < 4)
		Actions.push_back(new RL_Action(ai->utility->GetUnitDef("armlab")->GetUnitDefId(),1,false));
}

RL_State_Factory::~RL_State_Factory()
{
}

bool RL_State_Factory::IsTerminal()
{
	return true; // is this correct?
}
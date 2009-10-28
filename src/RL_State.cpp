#include "RL_State.h"
#include "RL.h"

using namespace brainSpace;

RL_State::RL_State( AIClasses* aiClasses, int plantCount, int labCount, int solarCount, int mexCount )
{
	ai = aiClasses;
	MexCount = mexCount;
	LabCount = labCount;
	SolarCount = solarCount;
	PlantCount = plantCount;
	ID = (PlantCount*2000)+(LabCount*400)+(20*MexCount)+SolarCount;

	if ( SolarCount < RL_SOLAR_INDEX )
		Actions.push_back( new RL_Action( ai->utility->GetUnitDef( "armsolar" )->GetUnitDefId(), 0 ) );
	if ( MexCount < RL_MEX_INDEX )
		Actions.push_back( new RL_Action( ai->utility->GetUnitDef( "armmex" )->GetUnitDefId(), 1 ) );
	if ( LabCount < RL_LAB_INDEX )
		Actions.push_back( new RL_Action( ai->utility->GetUnitDef( "armlab" )->GetUnitDefId(), 2 ) );
	if ( PlantCount < RL_PLANT_INDEX )
		Actions.push_back( new RL_Action( ai->utility->GetUnitDef( "armvp" )->GetUnitDefId(), 3 ) );
}

int RL_State::GetID()
{
	return ID;
}

vector<RL_Action*> RL_State::GetActions()
{
	return Actions;
}
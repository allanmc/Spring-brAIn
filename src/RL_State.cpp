#include "RL_State.h"
#include "RL.h"

using namespace brainSpace;

RL_State::RL_State( AIClasses* aiClasses, int labCount, int solarCount, int mexCount )
{
	ai = aiClasses;
	MexCount = mexCount;
	LabCount = labCount;
	SolarCount = solarCount;
	ID = (LabCount*400)+(20*MexCount)+SolarCount;

	if ( SolarCount < RL_SOLAR_INDEX )
	{
		Actions.push_back( new RL_Action( ai->utility->GetUnitDef( "armsolar" )->GetUnitDefId(), 0 ) );
	}
	if ( MexCount < RL_MEX_INDEX )
		Actions.push_back( new RL_Action( ai->utility->GetUnitDef( "armmex" )->GetUnitDefId(), 1 ) );
	if ( LabCount < RL_LAB_INDEX )
		Actions.push_back( new RL_Action( ai->utility->GetUnitDef( "armlab" )->GetUnitDefId(), 2 ) );
	//Actions.push_back( new RL_Action( -1, 3 ) );
}

int RL_State::GetID()
{
	return ID;
}

vector<RL_Action*> RL_State::GetActions()
{
	return Actions;
}
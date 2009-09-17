#include "Decision.h"
#include "Unit.h"
#include "UnitDef.h"

using namespace brainSpace;
using namespace springai;

brainSpace::Decision::Decision(AICallback* clb)
{
	this->callback = clb;
}

brainSpace::Decision::~Decision(void)
{
}

void Decision::UnitFinished(int unit)
{
	Unit * u = Unit::GetInstance(callback,unit);
	UnitDef * ud = u->GetDef();
	if(ud->IsAbleToMove())
	{
		//add to groupController
	}else{
		//add to BuildingController
	}
}

void Decision::UnitDestroyed(int unit, int attacker)
{
	Unit * u = Unit::GetInstance(callback,unit);
	if(u->GetDef()->IsAbleToMove())
	{
		//remove from groupController
	}else{
		//remove from BuildingController
	}

	//build a repacement?
}

void Decision::Update(int frame)
{
	if(frame == 1)
	{
		//build some crap
	}
}

#include "Decision.h"
#include "Unit.h"
#include "UnitDef.h"


using namespace brainSpace;
using namespace springai;

Decision::Decision(AICallback* clb)
{
	this->callback = clb;
	gc = new GroupController( clb );
}

Decision::~Decision(void)
{
}

void Decision::UnitFinished(int unit)
{

	char msg[200];
	Unit * u = Unit::GetInstance(callback,unit);

	UnitDef * ud = u->GetDef();
	if(ud->IsAbleToMove())
	{
		//add to groupController
		gc->AddUnit(u);
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
		gc->RemoveUnit(u);
	}else{
		//remove from BuildingController
	}

	//build a repacement?
}

void Decision::EnemyEnterLOS(int enemy)
{
	//kill that jerk!
}

void Decision::EnemyDestroyed(int enemy, int attacker)
{
	//good job!
}

void Decision::Update(int frame)
{
	if(frame == 1)
	{
		UnitDef* metalEx;
		UnitDef* solar;
		SBuildUnitCommand o;
		SBuildUnitCommand o2;
		for ( int i = 0 ; i < callback->GetUnitDefs().size() ; i++ )
		{
			if ( strcmp( callback->GetUnitDefs()[i]->GetName(), "armsolar" ) == 0 )
			{
				solar = callback->GetUnitDefs()[i];
			}
			else if ( strcmp( callback->GetUnitDefs()[i]->GetName(), "armmex" ) == 0 )
			{
				metalEx = callback->GetUnitDefs()[i];
			}
		}
		o.timeOut = 10000000;
		o.facing = 0;
		o.options = 0;
		o.toBuildUnitDefId = solar->GetUnitDefId();

		o2.timeOut = 10000000;
		o2.facing = 0;
		o2.options = 0;
		o2.toBuildUnitDefId = metalEx->GetUnitDefId();
		
		gc->ErectBuilding(o);
		gc->ErectBuilding(o2);
		gc->ErectBuilding(o);
		gc->ErectBuilding(o2);
		gc->ErectBuilding(o2);
		gc->ErectBuilding(o);
		//build some crap
		//find 2 nearest mex-spots
		//build mex at spot 1 (armmex)
		//build mex at spot 2 (armmex)
		//build solar (armsolar)
		//build K-bot lab (armlab)

		//spam mex and solar ()
		//spam kbots when lab is done (armflea)
	}
}

void Decision::UnitIdle( int id )
{
	Unit* u = Unit::GetInstance( callback, id );
	gc->UnitIdle( u );
}
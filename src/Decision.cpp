#include "Decision.h"
#include "Unit.h"
#include "UnitDef.h"


using namespace brainSpace;
using namespace springai;

Decision::Decision(AICallback* clb)
{
	this->callback = clb;
	gc = new GroupController( clb );
	bc = new BuildingController( clb);
}

Decision::~Decision(void)
{
}

void Decision::UnitFinished(int unit)
{

	char msg[200];
	Unit * u = Unit::GetInstance(callback,unit);

	UnitDef * ud = u->GetDef();
	if(ud->GetSpeed() > 0)
	{
		//add to groupController
		gc->AddUnit(u);
		if (!ud->IsBuilder())
		{
			BuildAttackUnit();
		}
	}else{
		//add to BuildingController
		bc->AddBuilding(u);
		BuildAttackUnit();
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
		bc->RemoveBuilding(u);
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
		Utility* u = new Utility(callback);
		u->ChatMsg("Frame 1");
		UnitDef* metalEx;
		UnitDef* solar, *kbotLab;
		SBuildUnitCommand solarOrder;
		SBuildUnitCommand metalExOrder, kbotLabOrder;
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
			else if ( strcmp( callback->GetUnitDefs()[i]->GetName(), "armlab" ) == 0 )
			{
				kbotLab = callback->GetUnitDefs()[i];
			}
		}
		solarOrder.timeOut = 10000000;
		solarOrder.facing = 0;
		solarOrder.options = 0;
		solarOrder.toBuildUnitDefId = solar->GetUnitDefId();

		metalExOrder.timeOut = 10000000;
		metalExOrder.facing = 0;
		metalExOrder.options = 0;
		metalExOrder.toBuildUnitDefId = metalEx->GetUnitDefId();
		
		kbotLabOrder.timeOut = 10000000;
		kbotLabOrder.facing = 0;
		kbotLabOrder.options = 0;
		kbotLabOrder.toBuildUnitDefId = kbotLab->GetUnitDefId();

	
		gc->ErectBuilding(metalExOrder);
		gc->ErectBuilding(solarOrder);
		gc->ErectBuilding(metalExOrder);
		gc->ErectBuilding(solarOrder);
		gc->ErectBuilding(metalExOrder);

		gc->ErectBuilding(kbotLabOrder);

		gc->ErectBuilding(solarOrder);
		gc->ErectBuilding(metalExOrder);
		gc->ErectBuilding(metalExOrder);
		gc->ErectBuilding(solarOrder);
		gc->ErectBuilding(metalExOrder);
		gc->ErectBuilding(metalExOrder);

		gc->ErectBuilding(kbotLabOrder);

		gc->ErectBuilding(metalExOrder);
		gc->ErectBuilding(solarOrder);
		gc->ErectBuilding(metalExOrder);
		gc->ErectBuilding(solarOrder);
		gc->ErectBuilding(metalExOrder);
		gc->ErectBuilding(metalExOrder);

		gc->ErectBuilding(kbotLabOrder);

		gc->ErectBuilding(metalExOrder);
		gc->ErectBuilding(solarOrder);
		gc->ErectBuilding(metalExOrder);
		gc->ErectBuilding(solarOrder);
		gc->ErectBuilding(metalExOrder);
		gc->ErectBuilding(solarOrder);
		

		u->ChatMsg( "Building erections planned" );
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

void Decision::BuildAttackUnit() {
	static UnitDef* unitToBuild = 0;
	Utility* utility = new Utility(callback);
	SBuildUnitCommand o;

	utility->ChatMsg("Trying to build attack unit...");
	if (!unitToBuild)
	{
		utility->ChatMsg("Searching for Rocko...");
		for ( int i = 0 ; i < callback->GetUnitDefs().size() ; i++ )
		{
			if ( strcmp( callback->GetUnitDefs()[i]->GetName(), "armrock" ) == 0 )
			{
				unitToBuild = callback->GetUnitDefs()[i];
				break;
			}
		}
	}

	if (unitToBuild)
	{
		utility->ChatMsg("Found Rocko, so building him...");
		o.timeOut = 10000000;
		o.facing = 0;
		o.options = 0;
		o.toBuildUnitDefId = unitToBuild->GetUnitDefId();
		bc->ConstructUnit(o);
	}



}
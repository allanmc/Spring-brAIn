#include "Decision.h"
#include "Unit.h"
#include "UnitDef.h"


using namespace brainSpace;
using namespace springai;

Decision::Decision(AIClasses* aiClasses)
{
	AI = aiClasses;
	gc = new GroupController( AI );
	bc = new BuildingController( AI );
}

Decision::~Decision(void)
{
}

void Decision::UnitFinished(int unit)
{

	char msg[200];
	Unit * u = Unit::GetInstance(AI->Callback,unit);
	AI->Utility->ChatMsg("unit pos:%f,%f", u->GetPos().x, u->GetPos().z);
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
	Unit * u = Unit::GetInstance(AI->Callback,unit);
	if(u->GetDef()->GetSpeed() > 0)
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
	gc->AttackWithGroup(enemy);
}

void Decision::EnemyDestroyed(int enemy, int attacker)
{
	//good job!
}

void Decision::Update(int frame)
{

	if(frame == 1)
	{
		AI->Utility->ChatMsg("Frame 1");
		UnitDef *solar, *kbotLab, *metalEx, *lltDef;
		SBuildUnitCommand metalExOrder, kbotLabOrder, solarOrder, lltDefOrder;
		for ( int i = 0 ; i < AI->Callback->GetUnitDefs().size() ; i++ )
		{
			if ( strcmp( AI->Callback->GetUnitDefs()[i]->GetName(), "armsolar" ) == 0 )
			{
				solar = AI->Callback->GetUnitDefs()[i];
			}
			else if ( strcmp( AI->Callback->GetUnitDefs()[i]->GetName(), "armmex" ) == 0 )
			{
				metalEx = AI->Callback->GetUnitDefs()[i];
			}
			else if ( strcmp( AI->Callback->GetUnitDefs()[i]->GetName(), "armlab" ) == 0 )
			{
				kbotLab = AI->Callback->GetUnitDefs()[i];
			}
			else if ( strcmp( AI->Callback->GetUnitDefs()[i]->GetName(), "armllt" ) == 0 ) 
			{
				lltDef = AI->Callback->GetUnitDefs()[i];
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

		lltDefOrder.timeOut = 10000000;
		lltDefOrder.facing = 0;
		lltDefOrder.options = 0;
		lltDefOrder.toBuildUnitDefId = lltDef->GetUnitDefId();

		gc->ErectBuilding(metalExOrder);
		gc->ErectBuilding(solarOrder);
		gc->ErectBuilding(metalExOrder);
		gc->ErectBuilding(solarOrder);
		gc->ErectBuilding(metalExOrder);
		gc->ErectBuilding(solarOrder);
		gc->ErectBuilding(kbotLabOrder);

		int num = 100;
		while (num--) {
			gc->ErectBuilding(lltDefOrder);
		}
		
		/*
		gc->ErectBuilding(solarOrder);
		gc->ErectBuilding(metalExOrder);
		gc->ErectBuilding(metalExOrder);
		gc->ErectBuilding(solarOrder);
		gc->ErectBuilding(metalExOrder);
		gc->ErectBuilding(metalExOrder);

		gc->ErectBuilding(kbotLabOrder);

		//gc->ErectBuilding(metalExOrder);
		//gc->ErectBuilding(solarOrder);
		//gc->ErectBuilding(metalExOrder);
		//gc->ErectBuilding(solarOrder);
		//gc->ErectBuilding(metalExOrder);
		//gc->ErectBuilding(metalExOrder);

		//gc->ErectBuilding(kbotLabOrder);

		//gc->ErectBuilding(metalExOrder);
		//gc->ErectBuilding(solarOrder);
		//gc->ErectBuilding(metalExOrder);
		//gc->ErectBuilding(solarOrder);
		//gc->ErectBuilding(metalExOrder);
		//gc->ErectBuilding(solarOrder);
		*/

		AI->Utility->ChatMsg( "Building erections planned" );
		//build some crap
		//find 2 nearest mex-spots
		//build mex at spot 1 (armmex)
		//build mex at spot 2 (armmex)
		//build solar (armsolar)
		//build K-bot lab (armlab)

		//spam mex and solar ()
		//spam kbots when lab is done (armflea)
	}

	if(frame % 60 == 0)
	{
		//u->ChatMsg("Scout command begin");
		gc->ScoutWithIdleGroup();
		//u->ChatMsg("Scout command sent");

	}


}

void Decision::UnitIdle( int id )
{
	Unit* u = Unit::GetInstance( AI->Callback, id );
	gc->UnitIdle( u );
}

void Decision::BuildAttackUnit() {
	static UnitDef* unitToBuild = 0;
	SBuildUnitCommand o;

	AI->Utility->ChatMsg("Trying to build attack unit...");
	if (!unitToBuild)
	{
		AI->Utility->ChatMsg("Searching for Rocko...");
		for ( int i = 0 ; i < AI->Callback->GetUnitDefs().size() ; i++ )
		{
			if ( strcmp( AI->Callback->GetUnitDefs()[i]->GetName(), "armrock" ) == 0 )
			{
				unitToBuild = AI->Callback->GetUnitDefs()[i];
				break;
			}
		}
	}

	if (unitToBuild)
	{
		AI->Utility->ChatMsg("Found Rocko, so building him...");
		o.timeOut = 10000000;
		o.facing = 0;
		o.options = 0;
		o.toBuildUnitDefId = unitToBuild->GetUnitDefId();
		bc->ConstructUnit(o);
	}



}

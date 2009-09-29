#include "Decision.h"
#include "Unit.h"
#include "UnitDef.h"


using namespace brainSpace;
using namespace springai;

Decision::Decision(AIClasses* aiClasses)
{
	ai = aiClasses;
	gc = new GroupController( ai );
	bc = new BuildingController( ai );
	BattleInfoInstance = new BattlesInfo( ai );
}

Decision::~Decision(void)
{
}

void Decision::UnitFinished(int unit)
{

	char msg[200];
	Unit * u = Unit::GetInstance(ai->callback,unit);
	
	ai->utility->ChatMsg("Unit finised, \"%s\", pos:%f,%f", u->GetDef()->GetName(), u->GetPos().x, u->GetPos().z);
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

	if (ud->GetWeaponMounts().size()>0) 
	{
		ai->knowledge->selfInfo->armyInfo->AddUnit(u);
	}
	else 
	{
		ai->knowledge->selfInfo->baseInfo->AddBuilding(u);
	}
}

void Decision::UnitDestroyed(int unit, int attacker)
{
	Unit * u = Unit::GetInstance(ai->callback,unit);
	ai->knowledge->selfInfo->armyInfo->RemoveUnit(u);
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

	Unit * unit = Unit::GetInstance(ai->callback,enemy);

	if (unit->GetDef()->GetWeaponMounts().size()>0) 
	{
		ai->knowledge->enemyInfo->armyInfo->UpdateUnit(unit);
		
	}
}

void Decision::EnemyDestroyed(int enemy, int attacker)
{
	//good job!
	Unit * unit = Unit::GetInstance(ai->callback,enemy);
	if (unit->GetDef()->GetWeaponMounts().size()>0) 
	{
		ai->knowledge->enemyInfo->armyInfo->RemoveUnit(unit);
	}
	else 
	{
		ai->knowledge->enemyInfo->baseInfo->RemoveBuilding(unit);
	}
}

void Decision::UpdateFrindlyPositions()
{
	
	vector<Unit*> units = ai->callback->GetFriendlyUnits();
	ArmyInfo* armyUnits = ai->knowledge->selfInfo->armyInfo;
	int unitCount = units.size();
	//Run though all frindly units
	for (int i; i < unitCount; i++)
	{
		//If position has changed compred to the one stored in the QuadTree, update the QuadTree.
		if (units[i]->GetPos().x != armyUnits->GetUnitPos(units[i]->GetUnitId()).x ||
			units[i]->GetPos().z != armyUnits->GetUnitPos(units[i]->GetUnitId()).z)
		{
			armyUnits->UpdateUnit( units[i] );
		}
	}
}

void Decision::Update(int frame)
{

	if(frame == 1)
	{
		ai->utility->ChatMsg("Frame 1");
		UnitDef *solar, *kbotLab, *metalEx, *lltDef;
		SBuildUnitCommand metalExOrder, kbotLabOrder, solarOrder, lltDefOrder;
		for ( int i = 0 ; i < ai->callback->GetUnitDefs().size() ; i++ )
		{
			if ( strcmp( ai->callback->GetUnitDefs()[i]->GetName(), "armsolar" ) == 0 )
			{
				solar = ai->callback->GetUnitDefs()[i];
			}
			else if ( strcmp( ai->callback->GetUnitDefs()[i]->GetName(), "armmex" ) == 0 )
			{
				metalEx = ai->callback->GetUnitDefs()[i];
			}
			else if ( strcmp( ai->callback->GetUnitDefs()[i]->GetName(), "armlab" ) == 0 )
			{
				kbotLab = ai->callback->GetUnitDefs()[i];
			}
			else if ( strcmp( ai->callback->GetUnitDefs()[i]->GetName(), "armllt" ) == 0 ) 
			{
				lltDef = ai->callback->GetUnitDefs()[i];
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

		ai->utility->ChatMsg( "Building erections planned" );
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

	if ( frame % 120 ==0 )
	{
		UpdateFrindlyPositions();
	}


}

void Decision::UnitIdle( int id )
{
	Unit* u = Unit::GetInstance( ai->callback, id );
	gc->UnitIdle( u );
}

void Decision::BuildAttackUnit() {
	static UnitDef* unitToBuild = 0;
	SBuildUnitCommand o;

	ai->utility->ChatMsg("Trying to build attack unit...");
	if (!unitToBuild)
	{
		ai->utility->ChatMsg("Searching for Rocko...");
		
		unitToBuild = ai->utility->GetUnitDef("armrock");
	}

	if (unitToBuild)
	{
		ai->utility->ChatMsg("Found Rocko, so building him...");
		o.timeOut = 10000000;
		o.facing = 0;
		o.options = 0;
		o.toBuildUnitDefId = unitToBuild->GetUnitDefId();
		bc->ConstructUnit(o);
	}



}

void Decision::UnitDamaged( int unitID, int attacker )
{
	BattleInfoInstance->UnitDamaged( unitID, attacker );
}

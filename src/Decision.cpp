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
	TM = new ThreatMap( ai );

	huginTest = new HuginTest( ai );
	huginTest->setEvidence("myStrategy", "Aggressive");
	huginTest->setEvidence("seenUnits", "0");
	huginTest->setEvidence("seenDef", "0");
	huginTest->setEvidence("seenProd", "0");
	huginTest->setEvidence("seenRes", "0");
	huginTest->setEvidence("attacks", "0");
	ai->utility->Log(ALL, BN, "My belief that that the enemy is aggressive: %f", huginTest->getBelief("enemyStrategy", "Aggressive"));
	ai->utility->Log(ALL, BN, "My belief that that the enemy is defensive: %f", huginTest->getBelief("enemyStrategy", "Defensive"));
}

Decision::~Decision(void)
{
}

void Decision::UnitCreated(int unitID, int builderID)
{
	Unit * u = Unit::GetInstance(ai->callback,unitID);
	Unit * builder = ( builderID ? Unit::GetInstance(ai->callback,builderID) : NULL);
	
	if(u->GetDef()->GetSpeed() == 0){//building
		float ETA = u->GetDef()->GetBuildTime() / builder->GetDef()->GetBuildSpeed();
		ai->knowledge->selfInfo->resourceInfo->AddChangeToCome(u,ETA);
	}
}

void Decision::UnitFinished(int unit)
{
	Unit * u = Unit::GetInstance(ai->callback,unit);
	
	ai->utility->Log(DEBUG, EVENT, "Unit finished, \"%s\", pos:%f,%f", u->GetDef()->GetName(), u->GetPos().x, u->GetPos().z);
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
		ai->knowledge->selfInfo->resourceInfo->RemoveChangeToCome(u);
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
	Unit* destroyee = Unit::GetInstance( ai->callback, unit );
	Unit* destroyer = Unit::GetInstance( ai->callback, attacker );
	BattleInfoInstance->UnitDestroyed( destroyee, destroyer );


	UnitDef* d = destroyee->GetDef();

	if ( d == NULL )
		ai->utility->Log( DEBUG, DECISION, "UnitDestroyed: UnitDef was null" );
	if ( d->GetUnitDefId() == -1 )
		ai->utility->Log( DEBUG, DECISION, "UnitDestroyed: Unitdef was -1" );


	if (destroyee->GetDef()->GetWeaponMounts().size()>0) 
	{
		ai->knowledge->selfInfo->armyInfo->RemoveUnit(destroyee);
	}
	else 
	{
		ai->knowledge->selfInfo->baseInfo->RemoveBuilding(destroyee);
	}


	if(destroyee->GetDef()->GetSpeed() > 0)
	{
		//remove from groupController
		gc->RemoveUnit(destroyee);
	}else{
		//remove from BuildingController
		bc->RemoveBuilding(destroyee);
		if(destroyee->IsBeingBuilt())
		{
			ai->knowledge->selfInfo->resourceInfo->RemoveChangeToCome(destroyee);
		}
	}

	//build a repacement?
}

void Decision::EnemyEnterLOS(int enemy)
{
	gc->AttackWithGroup(enemy);

	Unit * unit = Unit::GetInstance(ai->callback,enemy);
	UnitDef* d = unit->GetDef();
	
	if ( d == NULL )
		ai->utility->Log( DEBUG, DECISION, "EnemyEnterLOS: UnitDef was null" );
	if ( d->GetUnitDefId() == -1 )
		ai->utility->Log( DEBUG, DECISION, "EnemyEnterLOS: Unitdef was -1" );

	if (d->GetWeaponMounts().size()>0) 
	{
		ai->knowledge->enemyInfo->armyInfo->UpdateUnit(unit);
	} 
	else
	{
		ai->knowledge->enemyInfo->baseInfo->AddBuilding(unit);
	}
}

void Decision::EnemyDestroyed(int enemy, int attacker)
{
	//good job!
	Unit* unit = Unit::GetInstance(ai->callback, enemy);
	Unit* attackerUnit = Unit::GetInstance( ai->callback, attacker );
	UnitDef* d = unit->GetDef();

	if ( d == NULL )
		ai->utility->Log( DEBUG, DECISION, "EnemyDestroyed: UnitDef was null" );
	if ( d->GetUnitDefId() == -1 )
		ai->utility->Log( DEBUG, DECISION, "EnemyDestroyed: Unitdef was -1" );


	UnitDef* defPointer = NULL;

	BattleInfoInstance->EnemyDestroyed( unit, attackerUnit );

	if ( unit->GetDef()->GetUnitDefId() == -1 )
	{
		defPointer = ai->knowledge->enemyInfo->armyInfo->GetUnitDef( unit->GetUnitId() );
	}
	else defPointer = unit->GetDef();

	if ( defPointer == NULL ) //Unknown unit type: we never saw it, just killed it :D
	{
		return;
		/*
		ai->knowledge->enemyInfo->armyInfo->RemoveUnit( unit );
		ai->knowledge->enemyInfo->baseInfo->RemoveBuilding( unit );
		return;
		*/
	}

	if (defPointer->GetWeaponMounts().size()>0) 
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
	for (int i = 0; i < unitCount; i++)
	{
		//If position has changed compred to the one stored in the QuadTree, update the QuadTree.
		if (units[i]->GetPos().x != armyUnits->GetUnitPos(units[i]->GetUnitId()).x ||
			units[i]->GetPos().z != armyUnits->GetUnitPos(units[i]->GetUnitId()).z)
		{
			//armyUnits->UpdateUnit( units[i] );
		}
	}
}

void Decision::Update(int frame)
{

	if(frame == 1)
	{
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
	}

	if(frame % 60 == 0)
	{
		ai->utility->Log( DEBUG, KNOWLEDGE, "update" );
		TM->Update();
		ai->utility->Log( DEBUG, KNOWLEDGE, "update2" );
		gc->ScoutWithIdleGroup();
		BattleInfoInstance->Update( frame );
	}

	if ( frame % 120 ==0 )
	{

		UpdateFrindlyPositions();
		CBoundingBox box;
		box.topLeft.x = ai->callback->GetMap()->GetStartPos().x - 1000;
		box.topLeft.z = ai->callback->GetMap()->GetStartPos().z - 1000;
		box.bottomRight.x = ai->callback->GetMap()->GetStartPos().x + 1000;
		box.bottomRight.z = ai->callback->GetMap()->GetStartPos().z + 1000;
		//ai->utility->Log( DEBUG, KNOWLEDGE, "Start position (%f, %f)", ai->callback->GetMap()->GetStartPos().x, ai->callback->GetMap()->GetStartPos().z );
		//ai->utility->Log( DEBUG, KNOWLEDGE, "Number of battles close to our base within the last 6000 frames: %d. Current frame %d", BattleInfoInstance->NumberOfBattlesInArea( 6000, box ), ai->frame);
		int battles = BattleInfoInstance->NumberOfBattlesInArea( 9000, box );
		ai->utility->Log( DEBUG, KNOWLEDGE, "Number of battles close to our base within the last 9000 frames: %d", battles);
		const char* b_range;
		if(battles == 0)
		{
			b_range = "0";
		}
		else if(battles < 4)
		{
			b_range = "1-3";
		}
		else if(battles < 9)
		{
			b_range = "4-9";
		}
		else
		{
			b_range = "9-";
		}

		huginTest->setEvidence("attacks", b_range);
	}

	if ( frame % 120 == 60 )
	{
		//huginTest->setEvidence("myStrategy", "Aggressive");
		int enemyUnits = ai->knowledge->enemyInfo->armyInfo->CountAggressive();
		const char* u_range;
		if(enemyUnits == 0)
		{
			u_range = "0";
		}
		else if(enemyUnits < 11)
		{
			u_range = "1-10";
		}
		else if(enemyUnits < 51)
		{
			u_range = "11-50";
		}
		else
		{
			u_range = "51-300";
		}
		huginTest->setEvidence("seenUnits", u_range);
		ai->utility->ChatMsg("Seen units: %s", u_range);

		enemyUnits = ai->knowledge->enemyInfo->armyInfo->CountDefensive();
		if(enemyUnits == 0)
		{
			u_range = "0";
		}
		else if(enemyUnits < 6)
		{
			u_range = "1-5";
		}
		else if(enemyUnits < 11)
		{
			u_range = "6-10";
		}
		else
		{
			u_range = "11-20";
		}
		huginTest->setEvidence("seenDef", u_range);
		ai->utility->ChatMsg("Seen llt: %s", u_range);
		enemyUnits = ai->knowledge->enemyInfo->baseInfo->CountProductionBuildings();
		if(enemyUnits == 0)
		{
			u_range = "0";
		}
		else if(enemyUnits < 3)
		{
			u_range = "1-2";
		}
		else if(enemyUnits < 7)
		{
			u_range = "3-6";
		}
		else
		{
			u_range = "7-10";
		}
		huginTest->setEvidence("seenProd", u_range);
		ai->utility->ChatMsg("Seen production: %s", u_range);
		enemyUnits = ai->knowledge->enemyInfo->baseInfo->CountResourceBuildings();
		if(enemyUnits == 0)
		{
			u_range = "0";
		}
		else if(enemyUnits < 11)
		{
			u_range = "1-10";
		}
		else if(enemyUnits < 21)
		{
			u_range = "11-20";
		}
		else
		{
			u_range = "21-40";
		}
		huginTest->setEvidence("seenRes", u_range);
		ai->utility->ChatMsg("Seen resource: %s", u_range);
		ai->utility->Log(ALL, BN, "My belief that that the enemy is aggressive: %f", huginTest->getBelief("enemyStrategy", "Aggressive"));
		ai->utility->Log(ALL, BN, "My belief that that the enemy is defensive: %f", huginTest->getBelief("enemyStrategy", "Defensive"));
	}

	ai->knowledge->selfInfo->resourceInfo->Update(frame);
}

void Decision::UnitIdle( int id )
{
	Unit* u = Unit::GetInstance( ai->callback, id );
	gc->UnitIdle( u );
}

void Decision::BuildAttackUnit() {
	static UnitDef* unitToBuild = 0;
	SBuildUnitCommand o;

	if (!unitToBuild)
	{
		unitToBuild = ai->utility->GetUnitDef("armrock");
	}

	if (unitToBuild)
	{
		o.timeOut = 10000000;
		o.facing = 0;
		o.options = 0;
		o.toBuildUnitDefId = unitToBuild->GetUnitDefId();
		SAIFloat3 pos = {0,0,0};
		o.buildPos = pos;
		bc->ConstructUnit(o);
	}
}

void Decision::UnitDamaged( int unitID, int attacker )
{
	Unit* u1 = Unit::GetInstance( ai->callback, unitID );
	Unit* u2 = Unit::GetInstance( ai->callback, unitID );
	BattleInfoInstance->UnitDamaged( u1, u2 );
}

void Decision::EnemyDamaged( int attacker, int enemy )
{
	Unit* attackerUnit = Unit::GetInstance( ai->callback, attacker );
	Unit* enemyUnit = Unit::GetInstance( ai->callback, enemy );
	BattleInfoInstance->EnemyDamaged( attackerUnit, enemyUnit );
}

#include "Decision.h"
#include "Unit.h"
#include "UnitDef.h"
using namespace std;
using namespace springai;
using namespace brainSpace;

Decision::Decision(AIClasses* aiClasses)
{
	ai = aiClasses;
	
	gc = new GroupController( ai );
	bc = new BuildingController( ai );
	BattleInfoInstance = new BattlesInfo( ai );

	//time_t t1, t2, t3, t4, t5;
	//int i;
	//int iterations = 100;
	
	bn = new BayesianNetwork( ai );
	//huginTest = new HuginTest( ai );

	bn->setEvidence(node::_myStrategy, node::myStrategy::_Aggressive);
	bn->setEvidence(node::_seenUnits, node::seenUnits::_0);
	bn->setEvidence(node::_seenDef, node::seenDef::_0);
	bn->setEvidence(node::_seenProd, node::seenProd::_0);
	bn->setEvidence(node::_seenRes, node::seenRes::_0);
	bn->setEvidence(node::_attacks, node::attacks::_0);
	bn->Propagate();
	/*
	huginTest->setEvidence("myStrategy", "Aggressive");
	huginTest->setEvidence("seenUnits", "0");
	huginTest->setEvidence("seenDef", "0");
	huginTest->setEvidence("seenProd", "0");
	huginTest->setEvidence("seenRes", "0");
	huginTest->setEvidence("attacks", "0");
	huginTest->Propagate();
	*/

}

Decision::~Decision(void)
{
}

///called when a unit enters the world
///@warning the unit may not be fully constructed yet
///@warning the builderID may not corrospond to a unit
void Decision::UnitCreated(int unitID, int builderID)
{
	Unit * u = Unit::GetInstance(ai->callback,unitID);
	Unit * builder = ( builderID ? Unit::GetInstance(ai->callback,builderID) : NULL);
	
	if(u->GetDef()->GetSpeed() == 0){//building
		float ETA = u->GetDef()->GetBuildTime() / builder->GetDef()->GetBuildSpeed();
		ai->knowledge->selfInfo->resourceInfo->AddChangeToCome(u,ETA);
	}
}

///Called when a unit is fully constructed
///@param unit the ID of the unit
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

///called when one of our units are destoyed
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

///called when we can see an enemy unit that we could not see before.
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

///called when an enemy have been destroyed
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

///used to update the positions of all friendly units in the ArmyInfo
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
			armyUnits->UpdateUnit( units[i] );
		}
	}
}

void Decision::Update(int frame)
{

	if(frame == 1)
	{
		

			
/*
		vector<UnitDef*> p = ai->callback->GetUnitDefs();
		for ( int i = 0 ; i < p.size() ; i++ )
		{
			MoveData* d = p[i]->GetMoveData();
			if ( d == NULL )
			{
				ai->utility->Log( ALL, SLOPEMAP, "Unit data was null: %s - speed: %f, height: %f, waterline: %f, CanFly: %d", p[i]->GetHumanName(), p[i]->GetSpeed(), p[i]->GetHeight(), p[i]->GetWaterline(), p[i]->IsAbleToFly() );
				continue;
			}

			float slope = d->GetMaxSlope();
			float slopeMod = d->GetSlopeMod();
			ai->utility->Log( ALL, SLOPEMAP, "Unit %s: Max slope: %f, SlopeMod: %f, Max height diff: %f. Max pitch: %f", p[i]->GetHumanName(), slope, slopeMod, p[i]->GetMaxHeightDif(), p[i]->GetMaxPitch() );

		}
		*/
		ai->knowledge->mapInfo->resourceMap->Update();
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
		//ai->utility->Log( DEBUG, KNOWLEDGE, "pre-update" );
		ai->knowledge->mapInfo->scoutMap->Update();
		//ai->utility->Log( DEBUG, KNOWLEDGE, "update" );
		ai->knowledge->mapInfo->threatMap->Update();
		//ai->utility->Log( DEBUG, KNOWLEDGE, "update2" );
		gc->ScoutWithIdleGroup();
		//ai->utility->Log( DEBUG, KNOWLEDGE, "update3" );
		BattleInfoInstance->Update( frame );
		//ai->utility->Log( DEBUG, KNOWLEDGE, "update4" );
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
		//ai->utility->ChatMsg("Number of battles close to our base within the last 9000 frames: %d", battles);
		int b_range;
		if(battles == 0)
		{
			b_range = node::attacks::_0;
		}
		else if(battles < 4)
		{
			b_range = node::attacks::_1_3;
		}
		else if(battles < 9)
		{
			b_range = node::attacks::_4_8;
		}
		else
		{
			b_range = node::attacks::_9_;
		}

		//huginTest->setEvidence("attacks", b_range);
		bn->setEvidence(node::_attacks, b_range);
		
	}

	if (frame % 240 == 120)
	{
		ai->knowledge->selfInfo->baseInfo->DrawBasePerimiter();
		ai->knowledge->enemyInfo->baseInfo->DrawBasePerimiter();
	}

	if ( frame % 120 == 60 )
	{
		//huginTest->setEvidence("myStrategy", "Aggressive");
		int enemyUnits = ai->knowledge->enemyInfo->armyInfo->CountAggressive();
		int u_range;
		if(enemyUnits == 0)
		{
			u_range = node::seenUnits::_0;
		}
		else if(enemyUnits < 11)
		{
			u_range = node::seenUnits::_1_10;
		}
		else if(enemyUnits < 51)
		{
			u_range = node::seenUnits::_11_50;
		}
		else
		{
			u_range = node::seenUnits::_51_300;
		}
		//huginTest->setEvidence("seenUnits", u_range);
		bn->setEvidence(node::_seenUnits, u_range);

		ai->utility->Log(ALL, DECISION,"Seen units: %i", u_range);

		enemyUnits = ai->knowledge->enemyInfo->armyInfo->CountDefensive();
		if(enemyUnits == 0)
		{
			u_range = node::seenDef::_0;
		}
		else if(enemyUnits < 6)
		{
			u_range = node::seenDef::_1_5;
		}
		else if(enemyUnits < 11)
		{
			u_range = node::seenDef::_6_10;
		}
		else
		{
			u_range = node::seenDef::_11_20;
		}
		//huginTest->setEvidence("seenDef", u_range);
		bn->setEvidence(node::_seenDef, u_range);
		ai->utility->Log(ALL, DECISION,"Seen llt: %i", u_range);
		enemyUnits = ai->knowledge->enemyInfo->baseInfo->CountProductionBuildings();
		if(enemyUnits == 0)
		{
			u_range = node::seenProd::_0;
		}
		else if(enemyUnits < 3)
		{
			u_range = node::seenProd::_1_2;
		}
		else if(enemyUnits < 7)
		{
			u_range = node::seenProd::_3_6;
		}
		else
		{
			u_range = node::seenProd::_7_10;
		}
		//huginTest->setEvidence("seenProd", u_range);
		bn->setEvidence(node::_seenProd, u_range);
		ai->utility->Log(ALL, DECISION,"Seen production: %i", u_range);
		enemyUnits = ai->knowledge->enemyInfo->baseInfo->CountResourceBuildings();
		if(enemyUnits == 0)
		{
			u_range = node::seenRes::_0;
		}
		else if(enemyUnits < 11)
		{
			u_range = node::seenRes::_1_10;
		}
		else if(enemyUnits < 21)
		{
			u_range = node::seenRes::_11_20;
		}
		else
		{
			u_range = node::seenRes::_21_40;
		}
		//huginTest->setEvidence("seenRes", u_range);
		bn->setEvidence(node::_seenRes, u_range);
		ai->utility->Log(ALL, DECISION,"Seen resource: %i", u_range);
		ai->utility->Log(ALL, BN, "My belief that that the enemy is aggressive: %f", bn->getBelief(node::_enemyStrategy, node::enemyStrategy::_Aggressive));
		ai->utility->Log(ALL, BN, "My belief that that the enemy is defensive: %f", bn->getBelief(node::_enemyStrategy, node::enemyStrategy::_Defensive));
		//huginTest->print_beliefs_and_utilities();
	}
}

void Decision::UnitIdle( int id )
{
	Unit* u = Unit::GetInstance( ai->callback, id );
	gc->UnitIdle( u );

	//Construction groups has nothing to do... So build something we need!
	if (gc->ConstructionGroupIsIdle())
	{
		ai->utility->Log(ALL, GROUPING, "I have absolutely nothing to do now!");
	}
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

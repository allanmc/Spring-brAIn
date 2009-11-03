#include "Decision.h"
#include "Unit.h"
#include "UnitDef.h"
#include "Cheats.h"

using namespace std;
using namespace springai;
using namespace brainSpace;

int COMMANDERID = -1;
Decision::Decision(AIClasses* aiClasses)
{
	ai = aiClasses;
	
	gc = new GroupController( ai );
	ai->utility->Log(ALL, MISC, "GroupController loaded...");
	bc = new BuildingController( ai );
	ai->utility->Log(ALL, MISC, "BuildingController loaded...");
	rl = new RL( ai );
	ai->utility->Log(ALL, MISC, "RL loaded...");
	BattleInfoInstance = new BattlesInfo( ai );
	ai->utility->Log(ALL, MISC, "BattlesInfo loaded...");
	resettingGame = false;
	//time_t t1, t2, t3, t4, t5;
	//int i;
	//int iterations = 100;
	
	/*bn = new BayesianNetwork( ai );
	ai->utility->Log(ALL, MISC, "BayesianNetwork loaded...");
	//huginTest = new HuginTest( ai );

	bn->setEvidence(node::_myStrategy, node::myStrategy::_Aggressive);
	bn->setEvidence(node::_seenUnits, node::seenUnits::_0);
	bn->setEvidence(node::_seenDef, node::seenDef::_0);
	bn->setEvidence(node::_seenProd, node::seenProd::_0);
	bn->setEvidence(node::_seenRes, node::seenRes::_0);
	bn->setEvidence(node::_attacks, node::attacks::_0);
	bn->Propagate();*/
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
	delete gc;
	gc = NULL;
	delete bc;
	bc = NULL;
	delete rl;
	rl = NULL;
	delete BattleInfoInstance;
	BattleInfoInstance = NULL;
	//delete bn;
	//bn = NULL;
}

///called when a unit enters the world
///@warning the unit may not be fully constructed yet
///@warning the builderID may not corrospond to a unit
void Decision::UnitCreated(int unitID, int builderID)
{
	if (resettingGame)
	{
		return;
	}

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
	if (resettingGame)
	{
		return;
	}

	Unit * u = Unit::GetInstance(ai->callback,unit);
	
	ai->utility->ChatMsg("Unit finished, \"%s\", pos:%f,%f", u->GetDef()->GetName(), u->GetPos().x, u->GetPos().z);
	UnitDef * ud = u->GetDef();
	if(ud->GetSpeed() > 0)
	{
		//add to groupController
		gc->AddUnit(u);
		if (!ud->IsBuilder())
		{
			//BuildAttackUnit();
		}
	}else{
		//add to BuildingController
		bc->AddBuilding(u);
		//BuildAttackUnit();
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
	
	UpdateRL();
}

void Decision::UpdateRL()
{
	if(ai->frame > 0)
	{
		RL_Action *action = rl->Update();
		if ( action != NULL )
		{
			SBuildUnitCommand c;
			c.toBuildUnitDefId = action->Action;
			c.timeOut = 10000000;
			c.facing = 0;
			c.options = 0;
			gc->ErectBuilding( c );
			ai->utility->ChatMsg( "RL: Building unit with unitdef: %d", action->Action );
		}
		else 
		{
			ai->utility->ChatMsg( "we have reached our goal!!" );
			//ai->utility->Suicide();
			Reset();
		}
	}
}

///called when one of our units are destoyed
void Decision::UnitDestroyed(int unit, int attacker)
{
	ai->utility->ChatMsg( "UnitDestroyed id = %i, name = %s", unit, Unit::GetInstance(ai->callback,unit)->GetDef()->GetName() );
	if (resettingGame)
	{
		remainingUnits--;
		//vector<Unit*> units = ai->callback->GetFriendlyUnits();
		ai->utility->ChatMsg("Units in resetting game check: %i", remainingUnits);
		if (remainingUnits==0)//Are all old units destroyed now?
		{
			waitingForCommander = true;
		}
		/*else
		{
			ai->utility->Suicide(ai->commander->GetUnitId());
		}*/
		return;
	}

	Unit* destroyee = Unit::GetInstance( ai->callback, unit );
	Unit* destroyer = Unit::GetInstance( ai->callback, attacker );
	BattleInfoInstance->UnitDestroyed( destroyee, destroyer );


	UnitDef* d = destroyee->GetDef();

	if ( d == NULL )
		ai->utility->Log( LOG_DEBUG, DECISION, "UnitDestroyed: UnitDef was null" );
	else if ( d->GetUnitDefId() == -1 )
		ai->utility->Log( LOG_DEBUG, DECISION, "UnitDestroyed: Unitdef was -1" );


	if (destroyee->GetDef()->GetWeaponMounts().size()>0) 
	{
		ai->knowledge->selfInfo->armyInfo->RemoveUnit(destroyee);
	}
	else 
	{	
		ai->utility->ChatMsg( "Unit destroyed: %s", destroyee->GetDef()->GetHumanName() );
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
	if (resettingGame)
	{
		return;
	}

	gc->AttackWithGroup(enemy);

	Unit * unit = Unit::GetInstance(ai->callback,enemy);
	UnitDef* d = unit->GetDef();
	
	if ( d == NULL )
		ai->utility->Log( LOG_DEBUG, DECISION, "EnemyEnterLOS: UnitDef was null" );
	else if ( d->GetUnitDefId() == -1 )
		ai->utility->Log( LOG_DEBUG, DECISION, "EnemyEnterLOS: Unitdef was -1" );

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
	if (resettingGame)
	{
		return;
	}
	//good job!
	Unit* unit = Unit::GetInstance(ai->callback, enemy);
	Unit* attackerUnit = Unit::GetInstance( ai->callback, attacker );
	UnitDef* d = unit->GetDef();

	if ( d == NULL )
		ai->utility->Log( LOG_DEBUG, DECISION, "EnemyDestroyed: UnitDef was null" );
	else if ( d->GetUnitDefId() == -1 )
		ai->utility->Log( LOG_DEBUG, DECISION, "EnemyDestroyed: Unitdef was -1" );


	UnitDef* defPointer = NULL;

	BattleInfoInstance->EnemyDestroyed( unit, attackerUnit );

	if ( unit->GetDef() == NULL )
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

void Decision::Reset()
{
	resettingGame = true;
	waitingForCommander = false;
	vector<Unit*> units = ai->callback->GetFriendlyUnits();
	remainingUnits = units.size();
	ai->utility->ResetGame(&rl);
	delete ai->knowledge;
	ai->knowledge = NULL;
	ai->knowledge = new Knowledge( ai );
	delete ai->utility;
	ai->utility = NULL;
	ai->utility = new Utility( ai );
	delete ai->math;
	ai->utility->ChatMsg("Deleted ai->math");
	ai->math = NULL;
	ai->math = new BrainMath( ai );
	delete gc;
	ai->utility->ChatMsg("Deleted gc");
	gc = NULL;
	gc = new GroupController( ai );
	delete bc;
	ai->utility->ChatMsg("Deleted bc");
	bc = NULL;
	bc = new BuildingController( ai );
	ai->utility->ChatMsg("Reset() done");
}

void Decision::Update(int frame)
{
	//ai->utility->Log(ALL, MISC, "Currentframe: %i", frame);
	
	if (resettingGame) 
	{
		if (waitingForCommander)
		{
			ai->utility->Log(ALL, MISC, "Checking if new commander is in position X:(%f == %f), Z:(%f == %f)", ai->commander->GetPos().x, ai->callback->GetMap()->GetStartPos().x, ai->commander->GetPos().z, ai->callback->GetMap()->GetStartPos().z);
			if (abs(ai->commander->GetPos().x - ai->callback->GetMap()->GetStartPos().x)<50 &&
				abs(ai->commander->GetPos().z - ai->callback->GetMap()->GetStartPos().z)<50)
			{
				resettingGame = false;
				ai->utility->LaterInitialization();
				ai->knowledge->mapInfo->resourceMap->Update();
				UnitFinished(ai->commander->GetUnitId());
			}
		}
		/*ai->utility->ChatMsg("Hmm1");
		vector<Unit*> units = ai->callback->GetFriendlyUnits();
		ai->utility->ChatMsg("Hmm2");
		ai->utility->ChatMsg("Hmm3: %i", units.size());
		if (units.size()==1)
		{

			ai->utility->Log(ALL, MISC, "Checking if new commander is in position X:(%f == %f), Z:(%f == %f), units: %i", units[0]->GetPos().x, ai->callback->GetMap()->GetStartPos().x, units[0]->GetPos().z, ai->callback->GetMap()->GetStartPos().z, units.size());
			if (abs(units[0]->GetPos().x - ai->callback->GetMap()->GetStartPos().x)<50 &&
				abs(units[0]->GetPos().z - ai->callback->GetMap()->GetStartPos().z)<50)
			{
				resettingGame = false;
				ai->utility->LaterInitialization();
				ai->knowledge->mapInfo->resourceMap->Update();
				UnitFinished(units[0]->GetUnitId());
			}
		}
		else
		{
			//ai->utility->Suicide(ai->commander->GetUnitId());
		}*/
		return;
	}

	if(frame == 54000)
	{//kill your self after 30 mins
		//ai->utility->Suicide();
		//ai->utility->ResetGame(rl);
		//resettingGame = true;
	}
	if(frame == 1)
	{	
		ai->utility->ChatMsg( "I am now in frame 1!" );
		ai->utility->LaterInitialization();
		//ai->knowledge->mapInfo->scoutMap->DrawGrid();

		ai->knowledge->mapInfo->resourceMap->Update();
		//ai->knowledge->mapInfo->pathfindingMap->DrawGrid();
		//ai->knowledge->mapInfo->pathfindingMap->Update();
		//vector<Unit*> units = ai->callback->GetFriendlyUnits();
		//UnitFinished(units[0]->GetUnitId());
		UpdateRL();
		/*RL_Action *action = rl->Update();
		if ( action->ID != -1 )
		{
			SBuildUnitCommand c;
			c.toBuildUnitDefId = action->Action;
			c.timeOut = 10000000;
			c.facing = 0;
			c.options = 0;
			gc->ErectBuilding( c );
			ai->utility->ChatMsg( "RL: Building unit with unitdef: %d", action->Action );
		}*/
		

		/*
		
		UnitDef *solar, *kbotLab, *metalEx, *lltDef;
		SBuildUnitCommand metalExOrder, kbotLabOrder, solarOrder, lltDefOrder;
		for ( int i = 0 ; i < (int)ai->callback->GetUnitDefs().size() ; i++ )
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

		Unit *commander = ai->callback->GetFriendlyUnits()[0];
		COMMANDERID = commander->GetUnitId();
		kbotLabOrder.buildPos = commander->GetPos();
		kbotLabOrder.unitId = commander->GetUnitId();

		solarOrder.unitId = commander->GetUnitId();
		solarOrder.buildPos = ai->callback->GetMap()->FindClosestBuildSite( *solar, commander->GetPos(), 128, 0, 0 );
		solarOrder.options = UNIT_COMMAND_OPTION_SHIFT_KEY;
		for ( int i = 0 ; i < 6 ; i++ )
		{
			gc->ErectBuilding( solarOrder );

		}
		*/

		//kbotLabOrder.options = UNIT_COMMAND_OPTION_SHIFT_KEY;
		//ai->callback->GetEngine()->HandleCommand( 0, -1, COMMAND_UNIT_BUILD, &kbotLabOrder );
		/*kbotLabOrder.buildPos.z += ai->utility->GetUnitDef("armlab")->GetZSize() * 8;
		SMoveUnitCommand m;
		m.timeOut = 30000;
		m.toPos = kbotLabOrder.buildPos;
		m.unitId = commander->GetUnitId();
		m.options = UNIT_COMMAND_OPTION_SHIFT_KEY;
		ai->callback->GetEngine()->HandleCommand( 0, -1, COMMAND_UNIT_MOVE, &m );*/

		
		//gc->ErectBuilding(metalExOrder);
		//gc->ErectBuilding(solarOrder);
		//gc->ErectBuilding(metalExOrder);
		//gc->ErectBuilding(solarOrder);
		//gc->ErectBuilding(metalExOrder);
		//gc->ErectBuilding(solarOrder);
		//gc->ErectBuilding(kbotLabOrder);

		//int num = 100;
		//while (num--) {
		//	gc->ErectBuilding(lltDefOrder);
		//}
		
	}

	if(frame % 60 == 0)
	{
		//ai->knowledge->mapInfo->pathfindingMap->Update();
		//ai->utility->Log( LOG_DEBUG, KNOWLEDGE, "pre-update" );
		ai->knowledge->mapInfo->scoutMap->Update();
		//ai->utility->Log( LOG_DEBUG, KNOWLEDGE, "update" );
		ai->knowledge->mapInfo->threatMap->Update();
		//ai->utility->Log( LOG_DEBUG, KNOWLEDGE, "update2" );
		gc->ScoutWithIdleGroup();
		//ai->utility->Log( LOG_DEBUG, KNOWLEDGE, "update3" );
		BattleInfoInstance->Update( frame );
		//ai->utility->Log( LOG_DEBUG, KNOWLEDGE, "update4" );
	}

	if ( frame % 120 ==0 )
	{
		UpdateFrindlyPositions();
		CBoundingBox box;
		box.topLeft.x = ai->callback->GetMap()->GetStartPos().x - 1000;
		box.topLeft.z = ai->callback->GetMap()->GetStartPos().z - 1000;
		box.bottomRight.x = ai->callback->GetMap()->GetStartPos().x + 1000;
		box.bottomRight.z = ai->callback->GetMap()->GetStartPos().z + 1000;
		//ai->utility->Log( LOG_DEBUG, KNOWLEDGE, "Start position (%f, %f)", ai->callback->GetMap()->GetStartPos().x, ai->callback->GetMap()->GetStartPos().z );
		//ai->utility->Log( LOG_DEBUG, KNOWLEDGE, "Number of battles close to our base within the last 6000 frames: %d. Current frame %d", BattleInfoInstance->NumberOfBattlesInArea( 6000, box ), ai->frame);
		int battles = BattleInfoInstance->NumberOfBattlesInArea( 9000, box );
		ai->utility->Log( LOG_DEBUG, DECISION, "Number of battles close to our base within the last 9000 frames: %d", battles);
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
		//bn->setEvidence(node::_attacks, b_range);
		
	}

	if (frame % 240 == 120)
	{
		//ai->knowledge->selfInfo->baseInfo->DrawBasePerimiter();
		//ai->knowledge->enemyInfo->baseInfo->DrawBasePerimiter();
		//BuildSomethingUsefull();
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
		//bn->setEvidence(node::_seenUnits, u_range);

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
		//bn->setEvidence(node::_seenDef, u_range);
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
		//bn->setEvidence(node::_seenProd, u_range);
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
		/*
		bn->setEvidence(node::_seenRes, u_range);
		ai->utility->Log(ALL, DECISION,"Seen resource: %i", u_range);
		ai->utility->Log(ALL, BN, "My belief that that the enemy is aggressive: %f", bn->getBelief(node::_enemyStrategy, node::enemyStrategy::_Aggressive));
		ai->utility->Log(ALL, BN, "My belief that that the enemy is defensive: %f", bn->getBelief(node::_enemyStrategy, node::enemyStrategy::_Defensive));
		*/
		//huginTest->print_beliefs_and_utilities();
	}
}

void Decision::UnitIdle( int id )
{
	if (resettingGame)
	{
		return;
	}
	ai->utility->Log(ALL, MISC, "Decision::UnitIdle()");
	Unit* u = Unit::GetInstance( ai->callback, id );
	gc->UnitIdle( u );
	//BuildSomethingUsefull();
	//Construction groups has nothing to do... So build something we need!
}

void Decision::BuildSomethingUsefull()
{
	if (gc->ConstructionGroupIsIdle())
	{
		ai->utility->Log(ALL, MISC, "I have absolutely nothing to do now!");
		UnitDef *armsolar = ai->utility->GetSolarDef();
		//UnitDef *armmex = ai->utility->GetMexDef();
		//UnitDef *armlab = ai->utility->GetUnitDef("armlab");
		//UnitDef *armcom = ai->utility->GetUnitDef("armcom");
		//int isAffordable = 0;
		//ResourceInfo *ri = ai->knowledge->selfInfo->resourceInfo;
		SBuildUnitCommand buildOrder;
		buildOrder.timeOut = 10000000;
		buildOrder.facing = 0;
		buildOrder.options = 0;
		
		/*
		isAffordable = ri->IsAffordableToBuild(armcom, armlab);

		if (isAffordable==0)
		{
			buildOrder.toBuildUnitDefId = armlab->GetUnitDefId();
		}
		else if (isAffordable==-1)
		{
			buildOrder.toBuildUnitDefId = armmex->GetUnitDefId();
		}
		else if (isAffordable==-2 || isAffordable==-3)
		{
			buildOrder.toBuildUnitDefId = armsolar->GetUnitDefId();
		}
		else 
		{
			//buildOrder.toBuildUnitDefId = armmex->GetUnitDefId();
			ai->utility->Log(ALL, MISC, "I don't want to build anything right now... This shouldn't be the case:)");
			return;
		}
		ai->utility->Log(ALL, MISC, "I want to build: %s", UnitDef::GetInstance(ai->callback, buildOrder.toBuildUnitDefId)->GetName());
		*/
		buildOrder.toBuildUnitDefId = armsolar->GetUnitDefId();
		gc->ErectBuilding(buildOrder);
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
	if (resettingGame)
	{
		return;
	}
	Unit* u1 = Unit::GetInstance( ai->callback, unitID );
	Unit* u2 = Unit::GetInstance( ai->callback, unitID );
	BattleInfoInstance->UnitDamaged( u1, u2 );
}

void Decision::EnemyDamaged( int attacker, int enemy )
{
	if (resettingGame)
	{
		return;
	}
	Unit* attackerUnit = Unit::GetInstance( ai->callback, attacker );
	Unit* enemyUnit = Unit::GetInstance( ai->callback, enemy );
	BattleInfoInstance->EnemyDamaged( attackerUnit, enemyUnit );
}
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
	gameCounter = 0;
	ai->frame = 0;
	ai->utility->Log(ALL, MISC, "GroupController loaded...");
	bc = new BuildingController( ai );
	ai->utility->Log(ALL, MISC, "BuildingController loaded...");
	rl = new RL( ai, 0, EPSILON_START );
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
	ai->utility->ChatMsg("Starting game #%i", gameCounter++);
}

Decision::~Decision(void)
{
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
	UnitDef *uDef = u->GetDef();
	Unit * builder = ( builderID ? Unit::GetInstance(ai->callback,builderID) : NULL);
	UnitDef *bDef = NULL;
	if(builder != NULL)
	{
		bDef = builder->GetDef();
	}
	
	if(uDef->GetSpeed() == 0){//building
		float ETA = uDef->GetBuildTime() / bDef->GetBuildSpeed();
		ai->knowledge->selfInfo->resourceInfo->AddChangeToCome(u,ETA);
	}
	delete u;
	delete uDef;
	delete builder;
	delete bDef;
}

///Called when a unit is fully constructed
///@param unit the ID of the unit
void Decision::UnitFinished(int unit)
{
	if (resettingGame)
	{
		//return;
	}

	Unit *u = Unit::GetInstance(ai->callback,unit);
	
	UnitDef * ud = u->GetDef();
	vector<WeaponMount*> wpmt = ud->GetWeaponMounts();
	ai->utility->Log(ALL, MISC, "Unit finished, \"%s\", pos:%f,%f", ud->GetName(), u->GetPos().x, u->GetPos().z);
	if(ud->GetSpeed() > 0)
	{
		ai->utility->Log(ALL, MISC, "Finished with a non-building");
		//add to groupController
		ai->knowledge->groupManager->AddUnit(u);
		if (!ud->IsBuilder())
		{
			//BuildAttackUnit();
		}
	}else{
		ai->utility->Log(ALL, MISC, "Finished with a building");
		//add to BuildingController
		bc->AddBuilding(u);
		ai->utility->Log(ALL, MISC, "Added building");
		//BuildAttackUnit();
		ai->knowledge->selfInfo->resourceInfo->RemoveChangeToCome(u);
		ai->utility->Log(ALL, MISC, "Removed change to come");
	}

	if (wpmt.size()>0) 
	{
		ai->knowledge->selfInfo->armyInfo->AddUnit(u);
	}
	else 
	{
		ai->knowledge->selfInfo->baseInfo->AddBuilding(u);
	}
	ai->utility->Log(LOG_DEBUG, LOG_RL, "UnitFinished() returned");
	delete ud;
	for(int i = 0; i < (int)wpmt.size(); i++)
	{
		delete wpmt[i];
	}
	wpmt.clear();
	if(ai->frame > 0)
	{
		if(NEWSCENARIO && rl->ShouldIUpdate())
			UpdateRL();
		else if (!NEWSCENARIO)
			UpdateRL();
	}
}

void Decision::UpdateRL()
{
	if(ai->frame > 0)
	{
		RL_Action action = rl->Update();
		ai->utility->Log(LOG_DEBUG, LOG_RL, "RL:Update() returned");
		if ( action.ID != -1)
		{
			ai->utility->Log(LOG_DEBUG, LOG_RL, "UpdateRL() anot null action");
			ai->utility->Log(LOG_DEBUG, LOG_RL, "UpdateRL() action->ID = %i", action.ID);
			ai->utility->Log(LOG_DEBUG, LOG_RL, "UpdateRL() action->Action = %i", action.Action);
			if (action.Action == RL_ATTACK_ACTION)
			{
				//attack enemy
				ai->utility->Log(ALL, MISC, "Attacking enemy!!!");
				ai->utility->Log(ALL, MISC, "Rocko amount: %d", ai->knowledge->selfInfo->armyInfo->CountUnitsByName("armrock"));

				vector<Point*> points = ai->callback->GetMap()->GetPoints(false);
				SAIFloat3 enemyStartingPosition;
				for( int i = 0 ; i < (int)points.size() ; i++ )
				{					
					if ( points[i]->GetPosition().x == ai->callback->GetMap()->GetStartPos().x &&
						points[i]->GetPosition().z == ai->callback->GetMap()->GetStartPos().z )
					{
						continue;
					}
					enemyStartingPosition = points[i]->GetPosition();
					break;
				}
				Map *m = ai->callback->GetMap();
				enemyStartingPosition.x = 3971;
				enemyStartingPosition.z = 3976;
				ai->knowledge->groupManager->AttackPositionWithAllGroups(enemyStartingPosition);
			} 
			else
				{
				//simple unit to build
				
				SBuildUnitCommand c;
				ai->utility->Log(LOG_DEBUG, LOG_RL, "UpdateRL() action->Action = %i", action.Action);
				c.toBuildUnitDefId = action.Action;
				ai->utility->Log(LOG_DEBUG, LOG_RL, "UpdateRL() continuing");
				c.timeOut = 30000;

				if(action.ID == 3)
				{
					c.buildPos = (SAIFloat3){0,0,0};
					c.facing = UNIT_COMMAND_BUILD_NO_FACING;
					c.options = 0;
					bc->ConstructUnit(c);					
				}
				else
				{
					c.facing = 0;
					c.options = 0;
					ai->knowledge->groupManager->ErectBuilding( c );
				}
				ai->utility->Log(ALL, MISC,  "RL: Building unit with unitdef: %d", action.Action );
			}
		}
		else 
		{
			ai->utility->Log(ALL, MISC,  "we have reached our goal!!" );
			if (!NEWSCENARIO) {
				//ai->utility->Suicide();
				//Reset();
			}
		}
	}
	ai->utility->Log(LOG_DEBUG, LOG_RL, "UpdateRL() done");
}

///called when one of our units are destoyed
void Decision::UnitDestroyed(int unit, int attacker)
{
	ai->utility->Log(ALL, MISC,  "UnitDestroyed id = %i, name = %s", unit, Unit::GetInstance(ai->callback,unit)->GetDef()->GetName() );
	//if (resettingGame)
	//{
	//	remainingUnits--;
	//	//vector<Unit*> units = ai->callback->GetFriendlyUnits();
	//	ai->utility->Log(ALL, MISC, "Units in resetting game check: %i", remainingUnits);
	//	if (remainingUnits==0)//Are all old units destroyed now?
	//	{
	//		waitingForCommander = true;
	//	}
	//	/*else
	//	{
	//		ai->utility->Suicide(ai->commander->GetUnitId());
	//	}*/
	//	return;
	//}

	Unit* destroyee = Unit::GetInstance( ai->callback, unit );
	BattleInfoInstance->UnitDestroyed( unit, attacker );
	UnitDef* d = destroyee->GetDef();
	//Always remove :)
	ai->utility->Log(ALL, MISC, "armyInfo RemoveUnit ");
	ai->knowledge->selfInfo->armyInfo->RemoveUnit(unit);
	ai->utility->Log(ALL, MISC, "baseInfo RemoveUnit ");
	ai->knowledge->selfInfo->baseInfo->RemoveBuilding(unit);

	if ( d == NULL )
		ai->utility->Log( LOG_DEBUG, DECISION, "UnitDestroyed: UnitDef was null" );
	else if ( d->GetUnitDefId() == -1 )
		ai->utility->Log( LOG_DEBUG, DECISION, "UnitDestroyed: Unitdef was -1" );
	else
	{
		ai->utility->Log( LOG_DEBUG, DECISION, "UnitDestroyed: Unitdef was %s, iscommander: %d",d->GetName(), d->IsCommander() );
		if(NEWSCENARIO && d->IsCommander())
		{
			// our commander died
			ai->commanderDead = 2;
			UpdateRL();
		}
	}
	if(d->GetSpeed() > 0)
	{
		//remove from groupController
		ai->utility->Log(ALL, MISC, "groupManager RemoveUnit ");
		ai->knowledge->groupManager->RemoveUnit(destroyee);
	}else{
		//remove from BuildingController
		ai->utility->Log(ALL, MISC, "BuildingController RemoveBuilding ");
		bc->RemoveBuilding(destroyee);
		if(destroyee->IsBeingBuilt())
		{
			ai->knowledge->selfInfo->resourceInfo->RemoveChangeToCome(destroyee);
		}
	}
	delete d;
	delete destroyee;

}

///called when we can see an enemy unit that we could not see before.
void Decision::EnemyEnterLOS(int enemy)
{
	if (resettingGame)
	{
		return;
	}

	//ai->knowledge->groupManager->AttackWithGroup(enemy);

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
	UnitDef* d = unit->GetDef();
	if ( d == NULL )
	{
		d = ai->knowledge->enemyInfo->armyInfo->GetUnitDef( unit->GetUnitId() );
	}
	ai->knowledge->enemyInfo->armyInfo->RemoveUnit( enemy );
	ai->knowledge->enemyInfo->baseInfo->RemoveBuilding( enemy );
	if ( d == NULL ) //Unknown unit type: we never saw it, just killed it :D
	{		
		return;		
	}
	
	if ( d->GetUnitDefId() == -1 )
		ai->utility->Log( LOG_DEBUG, DECISION, "EnemyDestroyed: Unitdef was -1" );
	else
	{
		ai->utility->Log( LOG_DEBUG, DECISION, "EnemyDestroyed: Unitdef was %s",d->GetName() );
		if(NEWSCENARIO && d->IsCommander())
		{
			ai->utility->Log( LOG_DEBUG, DECISION, "EnemyDestroyed: commander" );
			//his commander died
			ai->commanderDead = 1;
			UpdateRL();
		}
	}
	BattleInfoInstance->EnemyDestroyed( enemy, attacker );
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
		if (units[i]->GetHealth() < 0.1)
		{
			continue;
		}
		//If position has changed compred to the one stored in the QuadTree, update the QuadTree.
		if (units[i]->GetPos().x != armyUnits->GetUnitPos(units[i]->GetUnitId()).x ||
			units[i]->GetPos().z != armyUnits->GetUnitPos(units[i]->GetUnitId()).z)
		{
				armyUnits->UpdateUnit( units[i] );
		}
		delete units[i];
	}
}

void Decision::Reset()
{
	if ( gameCounter == 40 )
	{
		ai->utility->Suicide();
		return;
	}
	resettingGame = true;
	waitingForCommander = false;
	extraMoveSent = false;
	vector<Unit*> units = ai->callback->GetFriendlyUnits();
	remainingUnits = units.size();
	ai->utility->ResetGame(&rl);
	ai->utility->ChatMsg("Starting game #%i", gameCounter++);
	delete ai->knowledge;
	ai->knowledge = NULL;
	ai->knowledge = new Knowledge( ai );
	delete ai->utility;
	ai->utility = NULL;
	ai->utility = new Utility( ai );
	delete ai->math;
	ai->utility->Log(ALL, MISC, "Deleted ai->math");
	ai->math = NULL;
	ai->math = new BrainMath( ai );
	delete bc;
	ai->utility->Log(ALL, MISC, "Deleted bc");
	bc = NULL;
	bc = new BuildingController( ai );
	ai->utility->Log(ALL, MISC, "Reset() done");
}

void Decision::Update(int frame)
{
	//ai->utility->Log(ALL, MISC, "Currentframe: %i", frame);
	
	if (resettingGame) 
	{
		if (waitingForCommander)
		{
			Map *map = ai->callback->GetMap();
			ai->utility->Log(ALL, MISC, "Checking if new commander is in position X:(%f == %f), Z:(%f == %f)", ai->commander->GetPos().x, ai->callback->GetMap()->GetStartPos().x, ai->commander->GetPos().z, ai->callback->GetMap()->GetStartPos().z);
			if (abs(ai->commander->GetPos().x - map->GetStartPos().x)<50 &&
				abs(ai->commander->GetPos().z - map->GetStartPos().z)<50)
			{
				resettingGame = false;
				ai->utility->LaterInitialization();
				ai->knowledge->mapInfo->resourceMap->Update();
				UnitFinished(ai->commander->GetUnitId());
			}
			else if (	!extraMoveSent &&
						ai->commander->GetPos().x == 10 &&
						ai->commander->GetPos().z == 10 ) //If we are waiting for the commander, but he hasn't move, then ask him again
			{
				SMoveUnitCommand moveCommand;
				moveCommand.toPos = ai->callback->GetMap()->GetStartPos();
				moveCommand.timeOut = 100000000;
				moveCommand.options = 0;
				moveCommand.unitId = ai->commander->GetUnitId();
				Engine *engine = ai->callback->GetEngine();
				engine->HandleCommand(0, -1, COMMAND_UNIT_MOVE, &moveCommand);
				extraMoveSent = true;
				delete engine;
			}
			delete map;
		}
		/*ai->utility->Log(ALL, MISC, "Hmm1");
		vector<Unit*> units = ai->callback->GetFriendlyUnits();
		ai->utility->Log(ALL, MISC, "Hmm2");
		ai->utility->Log(ALL, MISC, "Hmm3: %i", units.size());
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

	if(frame == 400000)
	{//kill your self after 30 mins

		// why didnt we finish??
		ai->utility->ChatMsg("We stopped.. fuck. ShouldIUpdate? %d", rl->ShouldIUpdate());
		//ai->knowledge->selfInfo->armyInfo->Print();
		//else
			//ai->utility->Suicide();
		//ai->utility->ResetGame(rl);
		//resettingGame = true;
	}

	if (NEWSCENARIO && frame % 90 == 0)
	{
		if (rl->ShouldIUpdate())
		{
			UpdateRL();
		}
	}
	if(frame == 1)
	{	
		ai->utility->Log(ALL, MISC,  "I am now in frame 1!" );
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
			ai->knowledge->groupManager->ErectBuilding( c );
			ai->utility->Log(ALL, MISC,  "RL: Building unit with unitdef: %d", action->UnitDefID );
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
			ai->knowledge->groupManager->ErectBuilding( solarOrder );

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

		
		//ai->knowledge->groupManager->ErectBuilding(metalExOrder);
		//ai->knowledge->groupManager->ErectBuilding(solarOrder);
		//ai->knowledge->groupManager->ErectBuilding(metalExOrder);
		//ai->knowledge->groupManager->ErectBuilding(solarOrder);
		//ai->knowledge->groupManager->ErectBuilding(metalExOrder);
		//ai->knowledge->groupManager->ErectBuilding(solarOrder);
		//ai->knowledge->groupManager->ErectBuilding(kbotLabOrder);

		//int num = 100;
		//while (num--) {
		//	ai->knowledge->groupManager->ErectBuilding(lltDefOrder);
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
		//ai->knowledge->groupManager->ScoutWithIdleGroup();
		//ai->utility->Log( LOG_DEBUG, KNOWLEDGE, "update3" );
		BattleInfoInstance->Update( frame );
		//ai->utility->Log( LOG_DEBUG, KNOWLEDGE, "update4" );
	}
	
	
	if ( frame % 120 ==0 )
	{
		UpdateFrindlyPositions();
		Map *map = ai->callback->GetMap();
		CBoundingBox box;
		box.topLeft.x = map->GetStartPos().x - 1000;
		box.topLeft.z = map->GetStartPos().z - 1000;
		box.bottomRight.x = map->GetStartPos().x + 1000;
		box.bottomRight.z = map->GetStartPos().z + 1000;
		delete map;
		//ai->utility->Log( LOG_DEBUG, KNOWLEDGE, "Start position (%f, %f)", ai->callback->GetMap()->GetStartPos().x, ai->callback->GetMap()->GetStartPos().z );
		//ai->utility->Log( LOG_DEBUG, KNOWLEDGE, "Number of battles close to our base within the last 6000 frames: %d. Current frame %d", BattleInfoInstance->NumberOfBattlesInArea( 6000, box ), ai->frame);
		int battles = BattleInfoInstance->NumberOfBattlesInArea( 9000, box );
		//ai->utility->Log( LOG_DEBUG, DECISION, "Number of battles close to our base within the last 9000 frames: %d", battles);
		//int b_range;
		//if(battles == 0)
		//{
		//	b_range = node::attacks::_0;
		//}
		//else if(battles < 4)
		//{
		//	b_range = node::attacks::_1_3;
		//}
		//else if(battles < 9)
		//{
		//	b_range = node::attacks::_4_8;
		//}
		//else
		//{
		//	b_range = node::attacks::_9_;
		//}

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
		//int enemyUnits = ai->knowledge->enemyInfo->armyInfo->CountAggressive();
		//int u_range;
		//if(enemyUnits == 0)
		//{
		//	u_range = node::seenUnits::_0;
		//}
		//else if(enemyUnits < 11)
		//{
		//	u_range = node::seenUnits::_1_10;
		//}
		//else if(enemyUnits < 51)
		//{
		//	u_range = node::seenUnits::_11_50;
		//}
		//else
		//{
		//	u_range = node::seenUnits::_51_300;
		//}
		////huginTest->setEvidence("seenUnits", u_range);
		////bn->setEvidence(node::_seenUnits, u_range);

		//ai->utility->Log(ALL, DECISION,"Seen units: %i", u_range);

		//enemyUnits = ai->knowledge->enemyInfo->armyInfo->CountDefensive();
		//if(enemyUnits == 0)
		//{
		//	u_range = node::seenDef::_0;
		//}
		//else if(enemyUnits < 6)
		//{
		//	u_range = node::seenDef::_1_5;
		//}
		//else if(enemyUnits < 11)
		//{
		//	u_range = node::seenDef::_6_10;
		//}
		//else
		//{
		//	u_range = node::seenDef::_11_20;
		//}
		//huginTest->setEvidence("seenDef", u_range);
		//bn->setEvidence(node::_seenDef, u_range);
		//ai->utility->Log(ALL, DECISION,"Seen llt: %i", u_range);
		//enemyUnits = ai->knowledge->enemyInfo->baseInfo->CountProductionBuildings();
		//if(enemyUnits == 0)
		//{
		//	u_range = node::seenProd::_0;
		//}
		//else if(enemyUnits < 3)
		//{
		//	u_range = node::seenProd::_1_2;
		//}
		//else if(enemyUnits < 7)
		//{
		//	u_range = node::seenProd::_3_6;
		//}
		//else
		//{
		//	u_range = node::seenProd::_7_10;
		//}
		////huginTest->setEvidence("seenProd", u_range);
		////bn->setEvidence(node::_seenProd, u_range);
		//ai->utility->Log(ALL, DECISION,"Seen production: %i", u_range);
		//enemyUnits = ai->knowledge->enemyInfo->baseInfo->CountResourceBuildings();
		//if(enemyUnits == 0)
		//{
		//	u_range = node::seenRes::_0;
		//}
		//else if(enemyUnits < 11)
		//{
		//	u_range = node::seenRes::_1_10;
		//}
		//else if(enemyUnits < 21)
		//{
		//	u_range = node::seenRes::_11_20;
		//}
		//else
		//{
		//	u_range = node::seenRes::_21_40;
		//}
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
	ai->utility->Log(ALL, MISC, "Decision::UnitIdle(), reset?: %d", resettingGame);
	/*if (resettingGame)
	{
		return;
	}*/
	
	Unit* u = Unit::GetInstance( ai->callback, id );
	ai->utility->Log(ALL,MISC,"Unit %s idle, but has %d commands", u->GetDef()->GetName(), u->GetCurrentCommands().size());
	if(u->GetCurrentCommands().size() == 0)
	{
		ai->knowledge->groupManager->UnitIdle( u );
	}
	if(u->GetUnitId() == ai->commander->GetUnitId())
	{
		if(NEWSCENARIO && rl->ShouldIUpdate())
			UpdateRL();
	}
	//BuildSomethingUsefull();
	//Construction groups has nothing to do... So build something we need!
	delete u;
}

void Decision::BuildSomethingUsefull()
{
	if (ai->knowledge->groupManager->ConstructionGroupIsIdle())
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
		ai->knowledge->groupManager->ErectBuilding(buildOrder);
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
	BattleInfoInstance->UnitDamaged( unitID, attacker );
}

void Decision::EnemyDamaged( int attacker, int enemy )
{
	if (resettingGame)
	{
		return;
	}
	BattleInfoInstance->EnemyDamaged( attacker, enemy );
}
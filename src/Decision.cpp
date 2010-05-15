#include "Decision.h"
#include "Unit.h"
#include "UnitDef.h"
#include "Cheats.h"
#include "WeaponMount.h"

using namespace std;
using namespace springai;
using namespace brainSpace;

int COMMANDERID = -1;
Decision::Decision(AIClasses* aiClasses)
{
	ai = aiClasses;
	GameCounter = 0;
	ai->frame = 0;
	bc = new BuildingController( ai );

	if ( ai->callback->GetTeamId() == 0 )
	{
		rl = new RL( ai, EPSILON_START );
		//rl->SetMayUpdate(true);
	}
	//BattleInfoInstance = new BattlesInfo( ai );
	ResettingGame = true;
	
	LastResetFrame = 0;
	m_Scenario = NULL;

	MayResetVariable = false;
	Suiciding = false;
}

Decision::~Decision(void)
{
	ai->utility->ChatMsg("Deleting bc");
	delete bc;
	bc = NULL;
	if ( ai->callback->GetTeamId() == 0 )
	{
	//	ai->utility->ChatMsg("Deleting rl");
		delete rl;
		rl = NULL;
	//	ai->utility->ChatMsg("Deleted RL");
	}
	//ai->utility->ChatMsg("Deleting BI");
	//delete BattleInfoInstance;
	//BattleInfoInstance = NULL;
	//delete bn;
	//bn = NULL;
}

///called when a unit enters the world
///@warning the unit may not be fully constructed yet
///@warning the builderID may not corrospond to a unit
void Decision::UnitCreated(int unitID, int builderID)
{
	if (ResettingGame)
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
		if ( builderID != -1 )
		{
			float ETA = uDef->GetBuildTime() / bDef->GetBuildSpeed();
			ai->knowledge->selfInfo->resourceInfo->AddChangeToCome(u,ETA);
		}
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
	Unit *u = Unit::GetInstance(ai->callback,unit);

	UnitDef * ud = u->GetDef();
	vector<WeaponMount*> wpmt = ud->GetWeaponMounts();
	ai->utility->Log(ALL, MISC, "Unit finished, \"%s\", pos:%f,%f", ud->GetName(), u->GetPos().x, u->GetPos().z);
	if(ud->GetSpeed() > 0)
	{
		//add to groupController
		//ai->utility->ChatMsg("Adding %d to GroupManager", unit );
		ai->knowledge->groupManager->AddUnit(u);
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
}

///called when one of our units are destoyed
void Decision::UnitDestroyed(int unit, int attacker)
{
	Unit* destroyee = Unit::GetInstance( ai->callback, unit );
	//BattleInfoInstance->UnitDestroyed( unit, attacker );
	UnitDef* d = destroyee->GetDef();

	if ( d->IsCommander() )
	{
		ai->utility->Suicide(0, true);
		Suiciding = true;
		ai->utility->ChatMsg("Our commander destroyed");
	}

	if(d->GetSpeed() > 0)
	{
		//remove from groupController
		ai->knowledge->groupManager->RemoveUnit(destroyee);
		ai->knowledge->selfInfo->armyInfo->RemoveUnit(unit );
	}else{
		//remove from BuildingController
		ai->utility->Log(ALL, MISC, "BuildingController RemoveBuilding ");
		bc->RemoveBuilding(destroyee);
		ai->knowledge->selfInfo->baseInfo->RemoveBuilding(unit);
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
	if (ResettingGame)
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
	//ai->utility->ChatMsg("EnemyDestroyed");
	if (ResettingGame)
	{
	//	ai->utility->ChatMsg("EnemyDestroyed, but returning");
		return;
	}
	//good job!
	Unit* unit = Unit::GetInstance(ai->callback, enemy);
	UnitDef* d = unit->GetDef();

	if ( d->IsCommander() )
	{
		ai->utility->Suicide(0, true);
		Suiciding = true;
		ai->utility->ChatMsg("Their commander destroyed");
	}

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
	}
	//BattleInfoInstance->EnemyDestroyed( enemy, attacker );
}

///used to update the positions of all friendly units in the ArmyInfo
void Decision::UpdateFrindlyPositions()
{
	vector<Unit*> units = ai->callback->GetFriendlyUnits();
	ArmyInfo* armyUnits = ai->knowledge->selfInfo->armyInfo;
	int unitCount = units.size();

	//ai->utility->ChatMsg("%d units in QuadTree", unitCount );
	//Run though all frindly units
	for (int i = 0; i < unitCount; i++)
	{
		if (units[i]->GetHealth() < 0.1f)
		{
			continue;
		}
		//If position has changed compred to the one stored in the QuadTree, update the QuadTree.
		if (units[i]->GetPos().x != armyUnits->GetUnitPos(units[i]->GetUnitId()).x ||
			units[i]->GetPos().z != armyUnits->GetUnitPos(units[i]->GetUnitId()).z)
		{
			UnitDef* d = units[i]->GetDef();
			//ai->utility->ChatMsg("Position has changed for %s", d->GetHumanName());
			//ai->utility->ChatMsg("Old (%f, %f, %f)", armyUnits->GetUnitPos(units[i]->GetUnitId()).x, armyUnits->GetUnitPos(units[i]->GetUnitId()).y, armyUnits->GetUnitPos(units[i]->GetUnitId()).z );
			//ai->utility->ChatMsg("New (%f, %f, %f)", units[i]->GetPos().x, units[i]->GetPos().y, units[i]->GetPos().z );
			delete d;
			armyUnits->UpdateUnit( units[i] );
		}
		delete units[i];
	}
}

void Decision::Reset()
{
//	ai->utility->ChatMsg("Decision::Reset()");
	GameCounter++;
	if ( ai->callback->GetTeamId() == 0 )
		if ( ( GameCounter % 10 ) == 0 )
			ai->utility->ChatMsg("DECISION: Resetting to game: %d", GameCounter );

	vector<Unit*> units = ai->callback->GetFriendlyUnits();
	if ( ai->callback->GetTeamId() == 0 )
	{
		ai->utility->ResetGame(&rl);
//		ai->utility->ChatMsg("Just Reset RL");
	}

	if ( GameCounter == 500 )
	{
		ai->utility->Suicide(COMMANDERID, true, true);
		Suiciding = true;
		return;
	}
	else
	{
		ai->utility->Suicide( COMMANDERID, false, true );
		Suiciding = true;
	}
	delete ai->knowledge;
	ai->knowledge = NULL;
	//ai->utility->ChatMsg("Deleted ai->knowledge");
	
	ai->knowledge = new Knowledge( ai );
	delete ai->utility;
	ai->utility = NULL;
	ai->utility = new Utility( ai );
	ai->utility->LaterInitialization();
	delete ai->math;
	//ai->utility->ChatMsg("Deleted ai->math");
	ai->math = NULL;
	ai->math = new BrainMath( ai );
	delete bc;
//	ai->utility->ChatMsg("Deleted bc");
	bc = NULL;
	bc = new BuildingController( ai );
//	ai->utility->ChatMsg("Reset() done");
	Suiciding = false;
}

void Decision::Update(int frame)
{
	if ( Suiciding )
		return;
	if(frame == 1)
	{
		ai->utility->ChatMsg("Frame 1!!");
		time_t t = time(NULL)*(ai->callback->GetTeamId()+2);

		srand((unsigned)t);
		ai->utility->Log(ALL, MISC,  "I am now in frame 1!" );
		ai->utility->LaterInitialization();
		ai->knowledge->mapInfo->resourceMap->Update();
		Unit *commander = ai->callback->GetFriendlyUnits()[0];
		Cheats* c = ai->callback->GetCheats();
		c->SetEnabled(true);
		delete c;
		c = NULL;
		COMMANDERID = commander->GetUnitId();
		SSetFireStateUnitCommand s;
		s.fireState = 0;
		s.unitId = COMMANDERID;
		s.timeOut = 1000000;
		delete commander;
		commander = NULL;
		NextScenarioStartFrame = frame+INITIAL_SCENARIO_DELAY;
	}


	
	if ( ResettingGame && frame == NextScenarioStartFrame )
	{
		LastResetFrame = frame;
		ResettingGame = false;
		//ai->utility->ChatMsg("New scenario");
		if ( m_Scenario != NULL )
		{		
			delete m_Scenario;
			m_Scenario = NULL;
		}
		ai->utility->ChatMsg("---------RESTARTING SCENARIO---------");
		m_Scenario = new Scenario(ai);
		if ( ai->callback->GetTeamId() == 0 )
			ai->utility->ChatMsg("Scenario started");
	}

	if ( ai->callback->GetTeamId() == 0 && frame == LastResetFrame+50  && !ResettingGame )
	{
		vector<Unit*> enemyUnits = ai->callback->GetEnemyUnits();
		for ( vector<Unit*>::iterator it = enemyUnits.begin() ; it != enemyUnits.end() ; it++ )
		{
			ai->knowledge->enemyInfo->armyInfo->UpdateUnit( *it );
			ai->knowledge->enemyInfo->baseInfo->AddBuilding( *it );
			delete (*it);
		}
		UpdateFrindlyPositions();

		ai->knowledge->mapInfo->threatMap->Update();
		vector<MilitaryUnitGroup*> m = ai->knowledge->groupManager->GetMilitaryGroupMgr()->GetAllAttackGroups();

		if ( m.size() > 0 )
		{
			RL_Action* a = rl->Update( m[0] );
			//ai->utility->ChatMsg("TEAM 0: FIRE AT WILL");
			if ( a != NULL )
			{
				m[0]->FireAtWill();
				if ( a->Path != NULL )
				{
					if ( a->Path->GetLength() > 0 )
					{
						for ( int i =  a->Path->GetPoints().size()-1 ; i >= 1 ; i-- )
						{
							m[0]->Scout( a->Path->GetPoints()[i] );
						}
					}
				}
				m[0]->Attack( a->unitIDs );
			}
			else
			{
				ai->utility->ChatMsg("DECISION: We got a state with no actions!");
				ai->utility->Suicide(0, true);
				Suiciding = true;
				return;
			}
		}
		else
		{
			ai->utility->ChatMsg("DECISION: Empty militaryunitgroup");
		}
	}

	if ( !ResettingGame )
	{
		MayResetVariable = rl->MayUpdate();
	}
	
	//Timeout or all units killed, so initiate a reset
	if ( frame > 1 && !ResettingGame && ( frame == LastResetFrame+RL_UPDATE_TIMEOUT || MayResetVariable ))
	{
		ResettingGame = true;
		LastResetFrame = frame;
		NextScenarioStartFrame = frame+SCENARIO_DELAY;
		if ( ai->callback->GetTeamId() == 0 )
		{
			vector<MilitaryUnitGroup*> m = ai->knowledge->groupManager->GetMilitaryGroupMgr()->GetAllAttackGroups();
			if ( m.size() == 0 )
			{		
				//ai->utility->ChatMsg("Decision: MiliGroup empty" );
				rl->Update( NULL );
			}
			else 
			{
				//ai->utility->ChatMsg("Decision: MiliGroupSize: %d", m.size() );
				rl->Update( m[0] );
			}
		}
		Reset();
	}
}

void Decision::UnitIdle( int id )
{
	Unit* u = Unit::GetInstance( ai->callback, id );
	//ai->utility->ChatMsg("Unit idle %d", id );
	//ai->utility->Log(ALL,MISC,"Unit %s idle, but has %d commands", u->GetDef()->GetName(), u->GetCurrentCommands().size());
	if(u->GetCurrentCommands().size() == 0)
	{
		if ( ai->knowledge->groupManager->UnitIdle( u ) )
		{
			if ( ai->callback->GetTeamId() == 0 )
			{
				//ai->utility->ChatMsg("DECISION: All units idle. ABOUT TO UPDATE RL");
				//rl->Update( ai->knowledge->groupManager->GetMilitaryGroupMgr()->GetIdleAttackGroups()[0] );
				//ai->utility->ChatMsg("DECISION: DONE UPDATING RL");
				
				//rl->SetMayUpdate(false);
				//rl->Update(Group);
				//RLDone = true;
			}
			//ai->utility->ChatMsg("All units in group idle. Restart scenario. Give reward. Whatever");
		}
	}
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
	if (ResettingGame)
	{
		return;
	}
	//BattleInfoInstance->UnitDamaged( unitID, attacker );
}

void Decision::EnemyDamaged( int attacker, int enemy )
{
	if (ResettingGame)
	{
		return;
	}
	//BattleInfoInstance->EnemyDamaged( attacker, enemy );
}
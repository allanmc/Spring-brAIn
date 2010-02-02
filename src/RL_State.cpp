#include "RL_State.h"
#include "RL.h"

using namespace brainSpace;

RL_State::RL_State()
{
}

RL_State::RL_State( AIClasses* ai, vector<RL_Action> actions, int id, bool terminal )
{
	Actions = actions;
	this->terminal = terminal;
	ID = id;
}


RL_State::RL_State( AIClasses* aiClasses, int node)
{
	Node = node;
	ai = aiClasses;
	switch(node)
	{
	case 0://root
		{
			ai->utility->Log(LOG_DEBUG, LOG_RL, "rl_state node 0");
			int prod = ai->knowledge->selfInfo->baseInfo->CountBuildingsByName("armlab") + ai->knowledge->selfInfo->baseInfo->CountBuildingsByName("armvp");
			int productionPower = (prod > 4 ? 4 : prod);
			ai->utility->Log(LOG_DEBUG, LOG_RL, "rl_state 1");
			int percent = ai->knowledge->mapInfo->scoutMap->GetScoutedPercentageWithinSeconds(300);
			int scoutKnowledge = (percent < 25 ? 0 : 1);
			scoutKnowledge += (percent < 50 ? 0 : 1);
			ai->utility->Log(LOG_DEBUG, LOG_RL, "rl_state 2");

			float energyProd = ai->knowledge->selfInfo->resourceInfo->GetCurrentProduction(ai->utility->GetResource("Energy"));
			float energyUse = ai->knowledge->selfInfo->resourceInfo->GetCurrentConsumption(ai->utility->GetResource("Energy"));
			float energyScale = energyProd/energyUse;
			int energyProfit = (energyScale < 0.5 ? 0 : 1);
			energyProfit += (energyScale < 1 ? 0 : 1);
			energyProfit += (energyScale < 1.5 ? 0 : 1);
			ai->utility->Log(LOG_DEBUG, LOG_RL, "rl_state 3");
			float metalProd = ai->knowledge->selfInfo->resourceInfo->GetCurrentProduction(ai->utility->GetResource("Metal"));
			float metalUse = ai->knowledge->selfInfo->resourceInfo->GetCurrentConsumption(ai->utility->GetResource("Metal"));
			float metalScale = metalProd/metalUse;
			int metalProfit = (metalScale < 0.5 ? 0 : 1);
			metalProfit += (metalScale < 1 ? 0 : 1);
			metalProfit += (metalScale < 1.5 ? 0 : 1);
			ai->utility->Log(LOG_DEBUG, LOG_RL, "rl_state 4");
			float knownEnemyAttackPower = (ai->knowledge->enemyInfo->armyInfo->GetAggresiveDps() > 0 ? ai->knowledge->enemyInfo->armyInfo->GetAggresiveDps() : 0.0001);;
			//hack because we have no defense :P
			float ourDefensePower = (ai->knowledge->selfInfo->armyInfo->GetAggresiveDps() > 0 ? ai->knowledge->selfInfo->armyInfo->GetAggresiveDps() : 0.0001);
			int enemyStrength = (knownEnemyAttackPower/ourDefensePower < 0.5 ? 0 : 1);
			enemyStrength += (knownEnemyAttackPower/ourDefensePower < 1 ? 0 : 1);
			enemyStrength += (knownEnemyAttackPower/ourDefensePower < 1.5 ? 0 : 1);
			ai->utility->Log(LOG_DEBUG, LOG_RL, "rl_state 5");
			//hack because we have no defense :P
			float knownEnemyDefensePower = (ai->knowledge->enemyInfo->armyInfo->GetAggresiveDps() > 0 ? ai->knowledge->enemyInfo->armyInfo->GetAggresiveDps() : 0.0001);;			
			float ourAttackPower = (ai->knowledge->selfInfo->armyInfo->GetAggresiveDps() > 0 ? ai->knowledge->selfInfo->armyInfo->GetAggresiveDps() : 0.0001);
			int ourStrength = (ourAttackPower/knownEnemyDefensePower < 0.5 ? 0 : 1);
			ourStrength += (ourAttackPower/knownEnemyDefensePower < 1 ? 0 : 1);
			ourStrength += (ourAttackPower/knownEnemyDefensePower < 1.5 ? 0 : 1);
			ai->utility->Log(LOG_DEBUG, LOG_RL, "rl_state 6");
			int commanderDead = ai->commanderDead;
			terminal = (commanderDead != 0);
			ai->utility->Log(LOG_DEBUG, LOG_RL, "rl_state 7");
			
			ID = enemyStrength*3*5*4*4*4*4 + ourStrength*3*5*4*4*4 + metalProfit*3*5*4*4 + energyProfit*3*5*4 + scoutKnowledge*3*5 + 3*productionPower + commanderDead;			

			Actions.push_back(RL_Action(ai->utility->GetUnitDef("armsolar")->GetUnitDefId(),0,false));
			Actions.push_back(RL_Action(ai->utility->GetUnitDef("armmex")->GetUnitDefId(),1,false));
			Actions.push_back(RL_Action(ai->utility->GetUnitDef("armlab")->GetUnitDefId(),2,false));
			Actions.push_back(RL_Action(ai->utility->GetUnitDef("armvp")->GetUnitDefId(),3,false));
			ai->utility->Log(LOG_DEBUG, LOG_RL, "rl_state 8");
			if(ai->knowledge->selfInfo->baseInfo->CountBuildingsByName("armlab") > 0)
			{
				ai->utility->Log(LOG_DEBUG, LOG_RL, "rl_state 81");
				Actions.push_back(RL_Action(ai->utility->GetUnitDef("armflea")->GetUnitDefId(),4,false));
				ai->utility->Log(LOG_DEBUG, LOG_RL, "rl_state 82");
				Actions.push_back(RL_Action(ai->utility->GetUnitDef("armrock")->GetUnitDefId(),5,false));
				ai->utility->Log(LOG_DEBUG, LOG_RL, "rl_state 83");
				Actions.push_back(RL_Action(ai->utility->GetUnitDef("armham")->GetUnitDefId(),6,false));
			}
			ai->utility->Log(LOG_DEBUG, LOG_RL, "rl_state 9");
			if(ai->knowledge->selfInfo->baseInfo->CountBuildingsByName("armvp") > 0)
			{
				Actions.push_back(RL_Action(ai->utility->GetUnitDef("armflash")->GetUnitDefId(),7,false));
				Actions.push_back(RL_Action(ai->utility->GetUnitDef("armfav")->GetUnitDefId(),8,false));
				Actions.push_back(RL_Action(ai->utility->GetUnitDef("tawf013")->GetUnitDefId(),9,false));
			}
			ai->utility->Log(LOG_DEBUG, LOG_RL, "rl_state 10");
			if(ai->knowledge->groupManager->GetMilitaryGroupMgr()->IsAttackGroupAvailable())
			{
				ai->utility->Log(LOG_DEBUG, LOG_RL, "RL-state: IsAttackGroupAvailable true");
				Actions.push_back(RL_Action(RL_ATTACK_BASE,10,false));
				Unit *uw = ai->knowledge->enemyInfo->baseInfo->GetWeakestBaseBuilding();
				if(uw != NULL)
					Actions.push_back(RL_Action(RL_ATTACK_WEAK,11,false));
				//Actions.push_back(RL_Action(RL_GUARD_COM,12,false));
				//Actions.push_back(RL_Action(RL_GUARD_WEAK,13,false));
			}
			ai->utility->Log(LOG_DEBUG, LOG_RL, "rl_state 11");
			if(ai->knowledge->groupManager->GetMilitaryGroupMgr()->IsScoutGroupAvailable())
				Actions.push_back(RL_Action(RL_SCOUT,12,false));		
			
		}break;
	default://error
		Node = -1;
	}
}

RL_State::~RL_State()
{
	Actions.clear();
}

int RL_State::GetID()
{
	return ID;
}

vector<RL_Action> RL_State::GetActions()
{
	ai->utility->Log(ALL, MISC, "RL_State::GetActions.size = %i", Actions.size());
	return Actions;
}

void RL_State::DeleteAction(int actionID)
{
	for (int i = 0; i < (int)Actions.size(); i++)
	{
		if(Actions[i].ID == actionID)
		{
			//delete Actions[i];
			//Actions[i] = NULL;
			Actions.erase(Actions.begin()+i);
			return;
		}
	}
}

bool RL_State::IsTerminal()
{
	return terminal;
}

int RL_State::GetNode()
{
	return Node;
}

bool RL_State::operator==(const RL_State &other) const
{
	return (ID == other.ID);
}

RL_State & RL_State::operator=(const RL_State &rhs)
{
	if (this == &rhs) // Same object?
      return *this;// Yes, so skip assignment, and just return *this.

	Actions = rhs.Actions;
	ID = rhs.ID;
	Node = rhs.Node;
	terminal = rhs.terminal;
	ai = rhs.ai;
	return *this;
}
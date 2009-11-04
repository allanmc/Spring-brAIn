#include "RL_State.h"
#include "RL.h"

using namespace brainSpace;

RL_State::RL_State( AIClasses* aiClasses)
{
	ai = aiClasses;
    ID = 0;//should be over written by the specific constructor
}

RL_State::~RL_State()
{
	for (int i = 0; i < (int)Actions.size(); i++)
	{
		delete Actions[i];
		Actions[i] = NULL;
	}
	Actions.clear();
}

int RL_State::GetID()
{
	return ID;
}

vector<RL_Action*> RL_State::GetActions()
{
	ai->utility->Log(ALL, MISC, "RL_State::GetActions.size = %i", Actions.size());
	return Actions;
}

void RL_State::DeleteAction(RL_Action* action)
{
	for (int i = 0; i < (int)Actions.size(); i++)
	{
		if(Actions[i]->ID == action->ID)
		{
			delete Actions[i];
			Actions[i] = NULL;
			Actions.erase(Actions.begin()+i);
			return;
		}
	}
}

bool RL_State::IsTerminal()
{
	return terminal;
}
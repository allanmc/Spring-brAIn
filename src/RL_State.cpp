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
	for (int i = 0; i < Actions.size(); i++)
	{
		delete(Actions[i]);
	}
	Actions.clear();
}

int RL_State::GetID()
{
	return ID;
}

vector<RL_Action*> RL_State::GetActions()
{
	return Actions;
}
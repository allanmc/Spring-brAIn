#ifndef _BRAINSPACE_RL_STATE_H
#define _BRAINSPACE_RL_STATE_H
#include <vector>
#include "game.h"
#include "RL_Action.h"

using namespace std;

namespace brainSpace {

	class RL_State
	{
	public:
		RL_State(Game *g, int agentId = 0);
		RL_State();
		virtual ~RL_State();
		int GetID();
		vector<RL_Action> GetActions();
		void DeleteAction(int actionID);
		bool IsTerminal();
		bool operator==(const RL_State &other) const;
		RL_State & operator=(const RL_State &rhs);
		
		static int lastLabCount;//hack for type = 2
		bool isBuildingLab;
	protected:
		Game *game;
		bool terminal;
		int ID;
		vector<RL_Action> Actions;
		int GetConCurId(int* actionCount);
		short unsigned int GetDiscrete(double min, double max, unsigned short int states, double value);
	};
}

#endif

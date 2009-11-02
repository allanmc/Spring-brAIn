#ifndef _BRAINSPACE_RL_STATE_H
#define _BRAINSPACE_RL_STATE_H

#include "types.h"

using namespace springai;
using namespace std;

namespace brainSpace {

	class RL_State
	{
	public:
		RL_State( AIClasses* ai, int plantCount, int labCount, int solarCount, int mexCount );

		int GetID();

		vector<RL_Action*> GetActions();
<<<<<<< HEAD
		void DeleteAction(RL_Action* action);
		virtual bool IsTerminal()=0;
	protected:
=======
		int LabCount;
	private:
>>>>>>> 996abe9cf5e17714255e2eb1b4a657304a2fddc9

		AIClasses* ai;

		int PlantCount;
		int SolarCount;
		int MexCount;
		int ID;
		vector<RL_Action*> Actions;

	};
}

#endif

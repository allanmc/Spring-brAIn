#ifndef _BRAINSPACE_DECISION_H
#define _BRAINSPACE_DECISION_H

#include "GroupController.h"
#include "BuildingController.h"
#include "AICallback.h"
#include "Utility.h"


using namespace springai;

namespace brainSpace {

class Decision
{
public:
	GroupController * gc;
	BuildingController * bc;

	void UnitFinished(int unit); 
	void UnitDestroyed(int unit, int attacker);
	void EnemyEnterLOS(int enemy);
	void EnemyDestroyed(int enemy, int attacker);
	void Update(int frame);

	Decision(AICallback* clb);
	~Decision(void);
private:
	AICallback* callback;
};

}

#endif // _BRAINSPACE_DECISION_H

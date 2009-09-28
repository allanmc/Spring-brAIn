#ifndef _BRAINSPACE_DECISION_H
#define _BRAINSPACE_DECISION_H

#include "GroupController.h"
#include "BuildingController.h"
#include "global.h"


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
	void UnitIdle( int unitID );
	void BuildAttackUnit();
	void UpdateFrindlyPositions();

	Decision(AIClasses* aiClasses);
	~Decision(void);
private:
	AIClasses* ai;
};

}

#endif // _BRAINSPACE_DECISION_H

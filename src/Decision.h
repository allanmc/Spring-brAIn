#ifndef _BRAINSPACE_DECISION_H
#define _BRAINSPACE_DECISION_H

#include "GroupController.h"
#include "BuildingController.h"
#include "global.h"
#include "BattlesInfo.h"
#include "ThreatMap.h"

using namespace springai;

namespace brainSpace {

class Decision
{
public:
	GroupController * gc;
	BuildingController * bc;
	
	void UnitCreated(int unit, int builder);
	void UnitFinished(int unit); 
	void UnitDestroyed(int unit, int attacker);

	void UnitDamaged(int unit, int attacker);
	void EnemyDamaged( int attacker, int enemy );

	void EnemyEnterLOS(int enemy);
	void EnemyDestroyed(int enemy, int attacker);
	void Update(int frame);
	void UnitIdle( int unitID );
	void BuildAttackUnit();
	void UpdateFrindlyPositions();

	Decision(AIClasses* aiClasses);
	~Decision(void);
private:

	BattlesInfo* BattleInfoInstance;
	AIClasses* ai;
	ThreatMap* TM;
};

}

#endif // _BRAINSPACE_DECISION_H

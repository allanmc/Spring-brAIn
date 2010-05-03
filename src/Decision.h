#ifndef _BRAINSPACE_DECISION_H
#define _BRAINSPACE_DECISION_H


#include "BuildingController.h"
#include "global.h"
#include "BattlesInfo.h"
#include "ThreatMap.h"
#include "Scenario.h"
#include "BrAIn.h"

using namespace springai;


namespace brainSpace {

///This class has the responsibillty to choose the apropriate actions, when an event occurs.
class Decision
{
public:
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
	void BuildSomethingUsefull();
	Decision(AIClasses* aiClasses);
	~Decision(void);

	bool MayResetVariable;
private:
	void Reset();
	BattlesInfo* BattleInfoInstance;
	AIClasses* ai;
	RL* rl;
	bool ResettingGame;
	int LastResetFrame;

	Scenario* m_Scenario;
	MilitaryUnitGroup* Group;
	int GameCounter;
};

}

#endif // _BRAINSPACE_DECISION_H

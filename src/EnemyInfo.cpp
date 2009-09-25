#include "EnemyInfo.h"


using namespace brainSpace;
using namespace springai;

EnemyInfo::EnemyInfo( AIClasses* aiClasses )
{
	ai = aiClasses;
	armyInfo = new ArmyInfo(ai);
	baseInfo = new BaseInfo(ai);
}

EnemyInfo::~EnemyInfo()
{
}

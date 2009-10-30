#include "EnemyInfo.h"


using namespace std;
using namespace springai;
using namespace brainSpace;

EnemyInfo::EnemyInfo( AIClasses* aiClasses )
{
	ai = aiClasses;
	armyInfo = new ArmyInfo(ai);
	baseInfo = new BaseInfo(ai);
}

EnemyInfo::~EnemyInfo()
{
	delete armyInfo;
	delete baseInfo;
	ai->utility->Log( ALL, MISC, "LOLOLOL" );
}

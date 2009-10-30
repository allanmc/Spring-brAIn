#include "SelfInfo.h"

using namespace std;
using namespace springai;
using namespace brainSpace;

SelfInfo::SelfInfo(AIClasses *aiClasses)
{
	ai = aiClasses;
	resourceInfo = new ResourceInfo(ai);
	armyInfo = new ArmyInfo(ai);
	baseInfo = new BaseInfo(ai);
}

SelfInfo::~SelfInfo()
{
	delete(resourceInfo);
	delete(armyInfo);
	delete(baseInfo);
}

#include "SelfInfo.h"


using namespace brainSpace;
using namespace springai;

SelfInfo::SelfInfo(AIClasses *ai)
{
	this->ai = ai;
	this->resourceInfo = new ResourceInfo(ai);
	armyInfo = new ArmyInfo(ai);
	baseInfo = new BaseInfo(ai);
}

SelfInfo::~SelfInfo()
{
}

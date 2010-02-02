#include "Knowledge.h"

using namespace std;
using namespace springai;
using namespace brainSpace;

Knowledge::Knowledge( AIClasses* aiClasses )
{
	ai = aiClasses;
	selfInfo = new SelfInfo(ai);
	enemyInfo = new EnemyInfo(ai);
	mapInfo = new MapInfo(ai);
	groupManager = new GroupController(ai);
}

Knowledge::~Knowledge()
{
	delete selfInfo;
	selfInfo = NULL;
	delete enemyInfo;
	enemyInfo = NULL;
	delete mapInfo;
	mapInfo = NULL;
	delete groupManager;
	groupManager = NULL;
}

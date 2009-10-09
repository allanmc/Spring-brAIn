#include "Knowledge.h"

using namespace std;
using namespace springai;
using namespace brainSpace;

Knowledge::Knowledge( AIClasses* aiClasses )
{
	ai = aiClasses;
	selfInfo = new SelfInfo(ai);
	enemyInfo = new EnemyInfo(ai);
}

Knowledge::~Knowledge()
{
}

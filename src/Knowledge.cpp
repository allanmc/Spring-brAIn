#include "Knowledge.h"

Knowledge::Knowledge( AIClasses* aiClasses )
{
	ai = aiClasses;
	selfInfo = new SelfInfo(ai);
	enemyInfo = new EnemyInfo(ai);
}

Knowledge::~Knowledge()
{
}

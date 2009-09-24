#include "Knowledge.h"

Knowledge::Knowledge( AIClasses* aiClasses )
{
	ai = aiClasses;
	selfInfo = new SelfInfo(aiClasses);
}

Knowledge::~Knowledge()
{
}

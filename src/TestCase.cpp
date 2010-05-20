#include "TestCase.h"

using namespace std;
using namespace brainSpace;
using namespace springai;

TestCase::TestCase( AIClasses* aiClasses )
{
	ai = aiClasses;
	Map* m = ai->callback->GetMap();
			
	switch(ai->callback->GetTeamId())
	{
	case 0:
		{
			SAIFloat3 pos = m->GetStartPos();
			for ( int i = -1 ; i <= 1 ; i++ )
			{
				for ( int j = -1 ; j <= 1 ; j++ )
				{
					pos.x = m->GetStartPos().x+(i*125);
					pos.z = m->GetStartPos().z+(j*125);
					delete ai->utility->GiveUnit("armmex", pos );
				}
			}
			break;
		}
	case 1:
		{
		break;
		}
	}
	delete m;
	m = NULL;
}

TestCase::~TestCase()
{
}
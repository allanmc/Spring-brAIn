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
			SAIFloat3 pos = (SAIFloat3) { 1300, 100, 1300 };
			for ( int i = -2 ; i < 0 ; i++ )
			{
				for ( int j = -2 ; j < 0 ; j++ )
				{
					SAIFloat3 tempPos = pos;			
					tempPos.x += i*45;
					tempPos.z += j*45;
					delete ai->utility->GiveUnit("armflash", tempPos );
				}
			}
			break;
		}
	case 1:
		{
			SAIFloat3 pos;
			pos.x = 250;
			pos.y = 100;
			pos.z = 250;
			for ( int i = -1 ; i < 1 ; i++ )
			{
				for ( int j = -1 ; j < 1 ; j++ )
				{
					SAIFloat3 tempPos = pos;
					tempPos.x += i*125;
					tempPos.z += j*125;
					ai->utility->ChatMsg("Spawning mex: (%f,%f,%f)", tempPos.x, tempPos.y, tempPos.z );
					delete ai->utility->GiveUnit("armmex", tempPos );
				}
			}

			pos.x = 500;
			pos.z = 500;
			for ( int i = -2 ; i < 2 ; i++ )
			{
				for ( int j = -2 ; j < 2 ; j++ )
				{
					SAIFloat3 tempPos = pos;			
					tempPos.x += i*45;
					tempPos.z += j*45;
					delete ai->utility->GiveUnit("armflash", tempPos );
				}
			}




			pos.x = 2350;
			pos.y = 100;
			pos.z = 750;
			for ( int i = -1 ; i < 1 ; i++ )
			{
				for ( int j = -1 ; j < 0 ; j++ )
				{
					SAIFloat3 tempPos = pos;
					tempPos.x += i*125;
					tempPos.z += j*125;
					delete ai->utility->GiveUnit("armmex", tempPos );
				}
			}


			break;
		}
	}
	delete m;
	m = NULL;
}

TestCase::~TestCase()
{
}
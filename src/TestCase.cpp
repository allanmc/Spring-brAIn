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
	case 0://Friendly units
		{
			SAIFloat3 pos = (SAIFloat3) { 1800, 100, 1300 };
			for ( int i = -2 ; i < 0 ; i++ )
			{
				for ( int j = -2 ; j < 0 ; j++ )
				{
					SAIFloat3 tempPos = pos;			
					tempPos.x += i*45;
					tempPos.z += j*45;
					
					ai->utility->ChatMsg("Spawning flash: (%f,%f,%f)", tempPos.x, tempPos.y, tempPos.z );
					delete ai->utility->GiveUnit("armflash", tempPos );
				}
			}
			break;
		}
	case 1://Enemy units
		{
			SAIFloat3 pos;
			
			
			// SCENARIO 1
			
			MakeMetalExtractors( 0, 0, 3 );
			MakeMetalExtractors( 6, 1, 3 );
			MakeMetalExtractors( 2, 4, 3 );
			MakeThreat( 0, 0, true );
			MakeThreat( 0, 1, true );
			MakeThreat( 1, 0, true );
			MakeThreat( 1, 1, true );


			MakeThreat( 1, 4 );
			MakeThreat( 2, 4 );
			MakeThreat( 1, 5 );
			MakeThreat( 2, 5 );
			

			/* SCENARIO 2 */
			
			/*
			MakeMetalExtractors( 0, 0, 3 );
			MakeMetalExtractors( 4, 1, 1 );
			MakeMetalExtractors( 2, 4, 4 );
			
			*/
			break;
		}
	}
	delete m;
	m = NULL;
}

TestCase::~TestCase()
{
}

void TestCase::MakeMetalExtractors( int tileX, int tileZ, int count )
{

	Map* m = ai->callback->GetMap();
	SAIFloat3 pos = m->GetStartPos();

	pos.x = (((float)tileX/(float)8)*4096.0f)+225.0f;
	pos.z = (((float)tileZ/(float)8)*4096.0f)+225.0f;

	delete m;
	m = NULL;

	int created = 0;
	for ( int i = -1 ; i < 1 ; i++ )
	{
		for ( int j = -1 ; j < 1 ; j++ )
		{
			if ( created == count )
				return;
			SAIFloat3 tempPos = pos;
			tempPos.x += j*125;
			tempPos.z += i*125;
			ai->utility->ChatMsg("Spawning mex: (%f,%f,%f)", tempPos.x, tempPos.y, tempPos.z );
			delete ai->utility->GiveUnit("armmex", tempPos );
			created++;
		}
	}
}


void TestCase::MakeThreat(int tileX, int tileZ, bool equal )
{
	Map* m = ai->callback->GetMap();
	SAIFloat3 tilecenter = (SAIFloat3) { tileX*512 + 256 , m->GetStartPos().y, tileZ*512 + 256 };
	delete m;
	m = NULL;

	if ( equal )
	{
		for ( int i = -1 ; i <= 1 ; i += 2 )
		{
			for ( int j = -1 ; j <= 1 ; j += 2 )
			{
				SAIFloat3 tmp = tilecenter;
				tmp.x += j*45;
				tmp.z += i*45;
				delete ai->utility->GiveUnit("armflash", tmp );
			}
		}
	
	}
	else
	{
		for ( int i = -1 ; i <= 1 ; i++ )
		{
			for ( int j = -1 ; j <= 1 ; j++ )
			{
				SAIFloat3 tmp = tilecenter;
				tmp.x += j*45;
				tmp.z += i*45;
				delete ai->utility->GiveUnit("armflash", tmp );
			}
		}
	}
}
#include "RL.h"

using namespace brainSpace;
using namespace std;
using namespace springai;

RL::RL( AIClasses* aiClasses)
{
	vector<float> f;
	
	ai = aiClasses;


	Actions.push_back( RL_Action( ai->utility->GetUnitDef( "armsolar" ), 0 ) );

	Actions.push_back( RL_Action( ai->utility->GetUnitDef( "armmex" ), 1 ) );

	Actions.push_back( RL_Action( ai->utility->GetUnitDef( "armlab" ), 2 ) );

	Actions.push_back( RL_Action( -1, 3 ) );


	if ( FileExists( "q.bin" ) )
	{
		ai->utility->Log( ALL, LOG_RL, "File exists, loading data" );
		ifstream readFile;

		readFile.open( "q.bin", ios::binary | ios::in );

		for ( int i = 0 ; i < RL_ACTION_INDEX ; i++ )
		{
			for ( int j = 0 ; j < RL_SOLAR_INDEX*RL_MEX_INDEX*RL_LAB_INDEX ; j++ )
			{
				float f;
				readFile.read( (char*)&f, sizeof(float) );
				RL_State s( ai, j/400, (j%400)/20, j%20 );
				ValueFunction.SetValue( s, Actions.at(i) );
			}
		}
		readFile.close();
	}
	else
	{
		ValueFunction = new RL_Q( RL_SOLAR_INDEX*RL_MEX_INDEX*RL_LAB_INDEX, RL_ACTION_INDEX );
	}

	Epsilon = 9;
	PreviousState = NULL;
	PreviousAction = NULL;
}

RL::~RL()
{
}

bool RL::FileExists( const char* FileName )
{
    FILE* fp = NULL;

	ai->utility->Log( ALL, LOG_RL, "Pik!" );
    fp = fopen( FileName, "rb" );
    if( fp != NULL )
    {
        fclose( fp );
        return true;
    }

    return false;
}

RL_State* RL::GetState()
{
	int solarCount = ai->knowledge->selfInfo->baseInfo->CountBuildingsByName( "armsolar" );
	int labCount = ai->knowledge->selfInfo->baseInfo->CountBuildingsByName( "armmex" );
	int mexCount = ai->knowledge->selfInfo->baseInfo->CountBuildingsByName( "armlab" );
	int state = solarCount+(labCount*400)+(mexCount*20);
	ai->utility->Log( ALL, LOG_RL, "Solar: %d. Lab: %d. Mex: %d. State: %d", solarCount, labCount, mexCount, state );
	return state;
}

int RL::FindNextAction( int state )
{
	int action = 0; //unitdefID
	
	int r = rand()%100;
	if ( r <= Epsilon ) //non-greedy
	{
		action = Actions[rand()%RL_ACTION_INDEX];
		ai->utility->Log( ALL, LOG_RL, "Non-greedy: %d", action );
	}
	else //greedy
	{
		float bestValue = Q[state][0];
		for ( int i = 1 ; i < RL_ACTION_INDEX ; i++ )
		{
			if ( Q[state][i] > bestValue )
			{
				bestValue = Q[state][i];
				action = i;
			}
		}
		ai->utility->Log( ALL, LOG_RL, "Greedy: %d", action );
	}
	return action;
}

int RL::FindBestAction( int state )
{
	int action = 0; //unitdefID

	float bestValue = Q[state][0];
	for ( int i = 1 ; i < RL_ACTION_INDEX ; i++ )
	{
		if ( Q[state][i] > bestValue )
		{
			bestValue = Q[state][i];
			action = i;
		}
	}

	return action;
}

int RL::Update( )
{
	bool terminal = false;
	int state = GetState();
	int nextAction = FindNextAction( state );
	int bestAction = FindBestAction( state );

	if ( PreviousState == -1 )
		return nextAction;

	int reward = 0;
	if ( ai->knowledge->selfInfo->baseInfo->CountBuildingsByName( "armlab" ) == 4 )
	{
		reward = 100;
		terminal = true;
	}


	Q[PreviousState][PreviousAction] = Q[PreviousState][PreviousAction] + ALPHA*(reward + GAMMA*( Q[state][bestAction] )  - Q[PreviousState][PreviousAction] );
	PreviousState = state;
	PreviousAction = nextAction;

	if ( terminal )
	{
		ofstream file( "q.bin", ios::binary | ios::out );
		for ( int i = 0 ; i < RL_ACTION_INDEX ; i++ )
		{
			for ( int j = 0 ; j < RL_SOLAR_INDEX ; j++ )
			{
				for ( int k = 0 ; k < RL_MEX_INDEX ; k++ )
			{
				for ( int l = 0 ; l < RL_LAB_INDEX ; l++ )
			{
				RL_State s( ai, l, j, k );
				RL_Action a(
				file.write( (char*)&Q[j][i], sizeof(float) );

			}
		
		}
		file.close();
		return -1;
	}
	return nextAction;
	
}
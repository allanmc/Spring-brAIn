#include <iostream>
#include "game.h"
#include "RL.h"
#include "RL_Q.h"
#include "RL_Action.h"
#include <time.h>
#include "main.h"

using namespace std;
using namespace brainSpace;

void ChangeColour(WORD theColour)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);  // Get handle to standard output
    SetConsoleTextAttribute(hConsole,theColour);  // set the text attribute of the previous handle
}

void PrintAction(bool debug,RL_Action a, unsigned short builder)
{
	if (debug)
	{
		switch(a.Action)
		{
			case LAB_ID:
				cerr << "L";
				break;
			case MEX_ID:
				cerr << "M";
				break;
			case SOLAR_ID:
				cerr << "S";
				break;
			case ROCKO_ID:
				cerr << "R";
				break;
		}
		cerr << builder << " ";
	}
}


int main(int argc, char *argv[])
{
	double currentEpsilon = EPSILON_START;
	srand((unsigned int)time(NULL));
	
	Game *g = new Game();
	RL *r;

	bool debug = true;

	cout << (USE_QSMDP ? "SMDPQ " : "Q ");

	if ( USE_Q_LAMBDA )
		cout << "Q("<< LAMBDA << ") ";

	if (USE_RS_TIME || USE_RS_TERMINATION)
	{
		cout << "w/ ";
		if(USE_RS_TIME)
		{
			printf("RS(Time)");
			if (USE_RS_TERMINATION)
				cout << " & ";
		}
		if (USE_RS_TERMINATION)
			cout << "RS(Termination)";
	}
	cout << " - " << (PRINT_REWARD ? "REWARD":"TIME") << "\n";
	double currentReward = 0.0;
	double currentFrame = 0.0;
	double bestFrame = 999999;
	double bestReward = -999999;
	int currentIndex = 0;
	int i = 0;
	int runs = 100000;
	while(i < runs)
	{
		g_currentGame = i;
		//if ( i % 500 == 0 )
		//	cerr << "Eps: " << currentEpsilon << endl;
		if ( i == runs-1  )
		{
			debug = true;
			currentEpsilon = 0.0f;
		}
			
		for ( unsigned int cTerm = 0 ; cTerm < (RL_TYPE==2?RL_LAB_INDEX-1:1) ; cTerm++ )
		{
			r = new RL(g, currentEpsilon, 2);
			RL_State::lastLabCount = g->units[LAB_ID];
			//Delete old Q-file?
			if ( i == 0 && RL_FILE_DELETE)
			{
				char* path = r->GetFilePath();
				if(remove(path) != 0)
					perror("deletion fail: ");
				delete[] path;
			}
			
			RL_Action a;
			a = r->Update(0);
			PrintAction(debug, a, 0);
			g->BuildUnit(a.Action, 0);
			a = r->Update(1);
			g->BuildUnit(a.Action, 1);
			PrintAction(debug, a, 1);

			bool terminal[] = {false, false};
			while( !terminal[0] || !terminal[1] )
			{
				vector<int> builders;
				while(true)
				{
					//game loop
					g->frame++;
					builders = g->Update();
					if(!builders.empty())
					{
						break;
					}
				}
				for(int i = 0; i < (int)builders.size(); i++)
				{
					a = r->Update(builders[i]);
					if ( a.ID != -1 ) 
					{
						g->BuildUnit(a.Action, builders[i]);
						PrintAction(debug, a, builders[i]);
					}
					else
					{
						if (debug) cerr << "T" << builders[i] << " ";
						terminal[builders[i]] = true;
					}
				}
			}
			currentReward += r->GetTotalReward();
			delete r;
		}
		if ( debug )
		{
			cerr << endl;
			cerr << "Metal: " << g->resources[MEX_ID] << endl;
			cerr << "Energy: " << g->resources[SOLAR_ID] << endl;
		}
		currentFrame = g->frame;
		bool goodNew = ( PRINT_REWARD ? bestReward <= currentReward : currentFrame <= bestFrame );
		if(goodNew)
		{
			ChangeColour(FOREGROUND_INTENSITY | FOREGROUND_GREEN); 
		} else {
			ChangeColour(FOREGROUND_INTENSITY | FOREGROUND_RED); 
		}
		
		g->ResetGame();
		
		if(PRINT_REWARD)
		{
			cout << currentReward << "\n" ;
		}
		else
		{
			cout << currentFrame << (currentFrame == bestFrame && debug ? "*" : "") << "\n" ;
		}
		currentIndex++;

		if(goodNew)
		{
			bestFrame = currentFrame;
			bestReward = currentReward;
			if (debug) system("pause");
		}

		currentReward = 0.0;
		currentFrame = 0.0;
		ChangeColour(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); 

		if(runs > 100 && i%(runs/100) == 0)
			cerr << "Status:" << floor(((i/(double)runs)*100)+0.5) << "%\xd";
		i++;

		currentEpsilon *= EPSILON_DECAY;
	}

	cerr << endl << "End epsilon: " << currentEpsilon << endl;
}

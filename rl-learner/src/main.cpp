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
		//cerr << builder;
	}
}


int main(int argc, char *argv[])
{
	double currentEpsilon = EPSILON_START;
	srand((unsigned int)time(NULL));
	
	Game *g = new Game();
	RL *r;

	bool debug = false;

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
		/*if ( i == 0 )
		{
			debug = true;
		}
		else if ( i == 1)
		{
			debug = false;
		}*/
		r = new RL(g, currentEpsilon, 2);
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

		bool terminal = false;
		while( !terminal )
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
				g->BuildUnit(a.Action, builders[i]);
				PrintAction(debug, a, builders[i]);
				if ( a.ID == -1 ) 
				{
					terminal = true;
				}
			}
		}
		if ( debug )
		{
			cout << endl;
		}
		currentReward += r->GetTotalReward();

		currentFrame = g->frame;
		if(currentFrame <= bestFrame)
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

		if(currentFrame <= bestFrame)
		{
			bestFrame = currentFrame;
			if (debug) system("pause");
		}

		currentReward = 0.0;
		currentFrame = 0.0;
		ChangeColour(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); 

		delete r;
		
		if(runs > 100 && i%(runs/100) == 0)
			cerr << "Status:" << floor(((i/(double)runs)*100)+0.5) << "%\xd";
		i++;

		currentEpsilon *= EPSILON_DECAY;
	}

	cerr << "\nend epsilon" << currentEpsilon << endl;
}

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

void PrintAction(bool debug,RL_Action a)
{
	if (debug)
	{
		switch(a.Action)
		{
			case LAB_ID:
				cout << "L";
				break;
			case MEX_ID:
				cout << "M";
				break;
			case SOLAR_ID:
				cout << "S";
				break;
			case ROCKO_ID:
				cout << "R";
				break;
		}
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

	if(USE_RS_TIME)
	{
		cout << "w/ ";
		printf("Reward Shaping(Time)");
	}

	cout << "\n";
	float currentReward = 0.0;
	float currentFrame = 0.0;
	float bestFrame = 999999;
	int currentIndex = 0;
	int i = 0;
	int runs = 10000;
	while(i < runs)
	{
		r = new RL(g, currentEpsilon, 2);
		//Delete old Q-file?
		if ( i == 0 && RL_FILE_DELETE)
		{
			char* path = r->GetFilePath();
			remove(path);
			delete[] path;
		}
		
		RL_Action a;
		a = r->Update(0);
		PrintAction(debug, a);
		g->BuildUnit(a.Action, 0);
		a = r->Update(1);
		g->BuildUnit(a.Action, 1);
		PrintAction(debug, a);

		bool terminal = false;
		while(!terminal)
		{
			float oldFrame = g->frame;
			
			vector<int> builders;
			while(true)
			{
				//game loop
				g->frame++;
				builders = g->Update();
				if(!builders.empty())
					break;
			}
			for(int i = 0; i < (int)builders.size(); i++)
			{
				a = r->Update(builders[i]);
				g->BuildUnit(a.Action, builders[i]);
				PrintAction(debug, a);
				if(a.ID == -1)
				{
					terminal = true;
				}
			}
			currentReward += r->GetTotalReward();
		}

		if ( debug )
		{
			cout << endl;
		}
		

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
}

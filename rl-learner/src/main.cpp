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
			case RL_LAB_ID:
				cout << "L";
				break;
			case RL_MEX_ID:
				cout << "M";
				break;
			case RL_SOLAR_ID:
				cout << "S";
				break;
			case RL_ROCKO_ID:
				cout << "R";
				break;
		}
	}
}


int main(int argc, char *argv[])
{
	double currentEpsilon = EPSILON_START;
	srand(time(NULL));
	int i = 0;
	Game *g = new Game();
	RL *r;

	//ChangeColour(FOREGROUND_GREEN); 

	//Delete old Q-file?
	if (RL_FILE_DELETE) 
	{
		const char* dir = RL_FILE_PATH;
		char *path = new char[200];
		strcpy(path, dir);
		strcat(path,	(RL_TYPE==0 ? RL_FILE_1 :
						(RL_TYPE==1 ? RL_FILE_2 :
						(RL_TYPE==2 ? RL_FILE_3 : ""))));
		remove(path);

	}
	bool debug = false;

	if ( USE_Q_LAMBDA )
		cout << "Q("<< LAMBDA << ")";
	else
		cout << (USE_QSMDP ? "SMDPQ " : "Q ");

	if(USE_RS_TIME || USE_RS_LABS || USE_BACKTRACKING)
		cout << "w/ ";
	if(USE_RS_TIME || USE_RS_LABS)
		printf("Reward Shaping(%s%s)",(USE_RS_TIME ? (USE_RS_LABS ? "Time,":"Time") : ""), (USE_RS_LABS ? "Labs" : "") );

	if(USE_BACKTRACKING)
	{
		cout << (USE_RS_TIME || USE_RS_LABS ? " & ": "") << "Backtracking(";
		if (BACKTRACKING_STEPS > 0)
			cout << BACKTRACKING_STEPS << ")";
		else
			cout << "Infinte" << ")";
	}
	if(USE_N_STEP)
	{
		cout << (USE_RS_TIME || USE_RS_LABS ? " & ": "") << "N-Step(";
		if (BACKTRACKING_STEPS > 0)
			cout << BACKTRACKING_STEPS << ")";
		else
			cout << "Infinte" << ")";
	}
	cout << "\n";
	//float bestReward = -1999;
	float currentReward = 0.0;
	float currentFrame = 0.0;
	float bestFrame = 999999;
	int currentIndex = 0;

	//while(true)
	//{
	//	//game loop
	//	g->frame++;
	//	bool update = g->Update();
	//	if(update)
	//	{
	//		RL_Action b = r->Update();
	//		if(b.ID == -1 || b.Action > 2)
	//		{
	//			g->ResetGame();
	//			continue;
	//		}
	//		g->BuildUnit(b.Action);

	//	}
	//	
	//	//if(runs > 100 && i%(runs/1000) == 0)
	//	//	cerr << "Status:" << floor(((i/(double)runs))+0.5) << "%\xd";
	//	//i++;


	//}


	unsigned int runs = 10000;
	while(i < runs)
	{
		r = new RL(g, RL_TYPE, currentEpsilon, 2);
		RL_Action a;
		a = r->Update(0);
		PrintAction(debug, a);
		g->BuildUnit(a.Action);
		a = r->Update(1);

		while(a.ID != -1)
		{
			PrintAction(debug, a);

			float oldFrame = g->frame;
			g->BuildUnit(a.Action);
			int builder;
			while(true)
			{
				//game loop
				g->frame++;
				builder = g->Update();
				if(builder != -1)
					break;
			}

			a = r->Update(builder);
		}
		if ( debug )
			cout << endl;
		currentReward += r->GetTotalReward();
		//cout << "Check: " << i << "\t" << r->GetTotalReward() << "\n" ;

		if (RL_TYPE != 2 || i%4==3) {
			currentFrame = g->frame;
			//if (currentReward >= bestReward)
			if(currentFrame <= bestFrame)
			{
				//bestReward = currentReward;
				ChangeColour(FOREGROUND_INTENSITY | FOREGROUND_GREEN); 
			} else {
				ChangeColour(FOREGROUND_INTENSITY | FOREGROUND_RED); 
			}
			/*
			if (debug) {
				float mNeeds_raw = g->BuildingCosts(RL_MEX_ID, buildingToBuild);
				short unsigned int mNeeds = g->GetDiscreteResource(mNeeds_raw);
				float eNeeds_raw = g->BuildingCosts(RL_SOLAR_ID, buildingToBuild);
				short unsigned int eNeeds = g->GetDiscreteResource(eNeeds_raw);
				float mAvailable_raw = g->AvailableResources(RL_MEX_ID, g->BuildTime(buildingToBuild));
				short unsigned int mAvailable = g->GetDiscreteResource(mAvailable_raw);
				float eAvailable_raw = g->AvailableResources(RL_SOLAR_ID, g->BuildTime(buildingToBuild));
				short unsigned int eAvailable = g->GetDiscreteResource(eAvailable_raw);
 
				cout << "Final State: ";
				cout << mNeeds << "(" << mNeeds_raw << ") - ";
				cout << eNeeds << "(" << eNeeds_raw << ") - ";
				cout << mAvailable << "(" << mAvailable_raw << ") - ";
				cout << eAvailable << "(" << eAvailable_raw << ")\n";
			}*/
			g->ResetGame();
			
			//if(i%120 == 3)
			if(PRINT_REWARD)
			{
				cout << currentReward << "\n" ;
			}
			else
			{
				cout << currentFrame << (currentFrame == bestFrame && debug ? "*" : "") << "\n" ;
			}
			currentIndex++;

			//if (currentReward >= bestReward)
			if(currentFrame <= bestFrame)
			{
				bestFrame = currentFrame;
				if (debug) system("pause");
			}

			currentReward = 0.0;
			currentFrame = 0.0;
			ChangeColour(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); 
		}
		delete r;
		
		if(runs > 100 && i%(runs/100) == 0)
			cerr << "Status:" << floor(((i/(double)runs)*100)+0.5) << "%\xd";
		i++;

		currentEpsilon *= EPSILON_DECAY;
	}
}

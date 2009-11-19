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


int main(int argc, char *argv[])
{
	srand(time(NULL));
	int i = 0;
	Game *g = new Game();
	RL *r;

	//ChangeColour(FOREGROUND_GREEN); 

	bool debug = false;

	//float bestReward = -1999;
	//float currentReward = 0.0;
	float currentFrame = 0.0;
	float bestFrame = 999999;
	int currentIndex = 0;
	short unsigned int runs = 10000;
	while(i < runs)
	{
		r = new RL(g, RL_TYPE);
		int buildingToBuild = RL_LAB_ID;
		r->setDesireToBuild(buildingToBuild);
		RL_Action a;
		a = r->Update();
		
		//if (i==0) system("pause");

		if (RL_TYPE == 2) {//We need to spend some resources
			//g->ConstructBuilding(RL_LAB_ID);
			//g->ConstructBuilding(RL_LAB_ID);
			//g->ConstructBuilding(RL_LAB_ID);
		}
/*
		if (i%4==0) {
			if (debug) {
				float mNeeds_raw = g->BuildingCosts(RL_MEX_ID, buildingToBuild);
				short unsigned int mNeeds = g->GetDiscreteResource(mNeeds_raw);
				float eNeeds_raw = g->BuildingCosts(RL_SOLAR_ID, buildingToBuild);
				short unsigned int eNeeds = g->GetDiscreteResource(eNeeds_raw);
				float mAvailable_raw = g->AvailableResources(RL_MEX_ID, g->BuildTime(buildingToBuild));
				short unsigned int mAvailable = g->GetDiscreteResource(mAvailable_raw);
				float eAvailable_raw = g->AvailableResources(RL_SOLAR_ID, g->BuildTime(buildingToBuild));
				short unsigned int eAvailable = g->GetDiscreteResource(eAvailable_raw);
			
				ChangeColour(FOREGROUND_INTENSITY | FOREGROUND_BLUE); 
				cout << "Initial State: ";
				cout << mNeeds << "(" << mNeeds_raw << ") - ";
				cout << eNeeds << "(" << eNeeds_raw << ") - ";
				cout << mAvailable << "(" << mAvailable_raw << ") - ";
				cout << eAvailable << "(" << eAvailable_raw << ")\n";
				ChangeColour(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); 
			}
		}
*/
		while(a.ID != -1)
		{
			if (RL_TYPE==2 && a.ID==0) break;
			if (debug) cout << (a.Action==RL_MEX_ID?"M":(a.Action == RL_SOLAR_ID ? "E" : "L"));

			float oldFrame = g->frame;
			g->ConstructBuilding(a.Action);

			a = r->Update();		
		}
		
		if (RL_TYPE == 2)
		{
			if (debug) cout << "L";
			g->ConstructBuilding(buildingToBuild);
			a = r->Update();
		} 
		if (debug) cout << "\n";
		//currentReward += r->GetTotalReward();
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
			//cout << currentIndex-1 << "\t" << currentReward << "\n" ;
			cout << currentIndex++ << "\t" << currentFrame << (currentFrame == bestFrame && debug ? "*" : "") << "\n" ;
			//currentIndex++;

			//if (currentReward >= bestReward)
			if(currentFrame <= bestFrame)
			{
				bestFrame = currentFrame;
				if (debug) system("pause");
			}

			//currentReward = 0.0;
			currentFrame = 0.0;
			ChangeColour(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); 
		}
		delete r;
		
		if(i%(runs/100) == 0)
			cerr << "Status:" << round(((i/(double)runs)*100)) << "%\n";
		i++;
	}
	//system("pause");
}

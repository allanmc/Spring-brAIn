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

	float bestReward = -1999;
	float currentReward = 0.0;
	while(i < 400)
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
		while(a.ID != -1)
		{
			if (RL_TYPE==2 && a.ID==0) break;
			cout << (a.Action==RL_MEX_ID?"M":"E");

			float oldFrame = g->frame;
			g->ConstructBuilding(a.Action);

			a = r->Update();		
		}
		
		if (RL_TYPE == 2)
		{
			cout << "L\n";
			g->ConstructBuilding(buildingToBuild);
			a = r->Update();
		} 
		
		currentReward += r->GetTotalReward();
		//cout << "Check: " << i << "\t" << r->GetTotalReward() << "\n" ;

		if (i%4==3) {
			g->ResetGame();

			if (currentReward >= bestReward)
			{
				bestReward = currentReward;
				ChangeColour(FOREGROUND_INTENSITY | FOREGROUND_GREEN); 
			} else {
				ChangeColour(FOREGROUND_INTENSITY | FOREGROUND_RED); 
			}

			short unsigned int mNeeds = g->GetDiscreteResource(g->BuildingCosts(RL_MEX_ID, buildingToBuild));
			short unsigned int eNeeds = g->GetDiscreteResource(g->BuildingCosts(RL_SOLAR_ID, buildingToBuild));
			short unsigned int mAvailable = g->GetDiscreteResource(g->AvailableResources(RL_MEX_ID, g->BuildTime(buildingToBuild)));
			short unsigned int eAvailable = g->GetDiscreteResource(g->AvailableResources(RL_SOLAR_ID, g->BuildTime(buildingToBuild)));
			
			cout << "Current State: " << mNeeds << " - " << eNeeds << " - " << mAvailable << " - " << eAvailable << "\n";
			

			//if(i%120 == 3)
			cout << i << "\t" << currentReward << "\n" ;

			if (currentReward >= bestReward)
			{
				system("pause");
			}

			currentReward = 0.0;
			ChangeColour(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); 
		}

		delete r;
		//if(i%100 == 0)
			//cout << "I:" << i << "\n";
		i++;
	}
	system("pause");
}

#include <iostream>
#include "game.h"
#include "RL.h"
#include "RL_Q.h"
#include "RL_Action.h"
#include <time.h>

using namespace std;
using namespace brainSpace;
int main(int argc, char *argv[])
{
	srand(time(NULL));
	int i = 20000;
	Game *g = new Game();
	while(i--)
	{
		RL *r = new RL(g);
		RL_Action a;
		a = r->Update();
		while(a.ID != -1)
		{
			//cout << "real-frame before construct: " << g->frame << "\n";
			g->ConstructBuilding(a.Action);
			a = r->Update();		
		}
		//cout << "Totalreward" << r->GetTotalReward() << "\n";
		delete r;
		g->ResetGame();
	}
}

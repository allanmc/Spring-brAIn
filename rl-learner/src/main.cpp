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
	int i = 0;
	Game *g = new Game();
	RL *r;
	float bestReward = -1999;
	while(i < 1000)
	{
		r = new RL(g, RL_TYPE);
		RL_Action a;
		a = r->Update();	
		//vector<RL_Action> actions;
		//vector<float> resource1;
		//vector<float> resource2;
		//vector<float> rewards;
		
		if (RL_TYPE == 2) {//We need to spend some resources
			g->ConstructBuilding(RL_LAB_ID);
			g->ConstructBuilding(RL_LAB_ID);
			g->ConstructBuilding(RL_MEX_ID);
			g->ConstructBuilding(RL_MEX_ID);
			g->ConstructBuilding(RL_MEX_ID);
			g->ConstructBuilding(RL_MEX_ID);
			g->ConstructBuilding(RL_MEX_ID);
			g->ConstructBuilding(RL_MEX_ID);
			g->ConstructBuilding(RL_MEX_ID);
			g->ConstructBuilding(RL_MEX_ID);
			g->ConstructBuilding(RL_MEX_ID);
			//g->ConstructBuilding(RL_SOLAR_ID);
			//g->ConstructBuilding(RL_SOLAR_ID);
			//g->ConstructBuilding(RL_SOLAR_ID);
			g->ConstructBuilding(RL_SOLAR_ID);
		}
		while(a.ID != -1)
		{
			if (RL_TYPE==2 && a.ID==0) break;
			cout << (a.Action==RL_MEX_ID?"M":"E");

			//actions.push_back(a);
			//cout << "real-frame before construct: " << g->frame << "\n";
			//resource1.push_back(g->resources[RL_SOLAR_ID]);
			//resource2.push_back(g->resources[RL_MEX_ID]);
			
			float oldFrame = g->frame;
			g->ConstructBuilding(a.Action);
			//cout << "Frame duration: " << g->frame - oldFrame << "\n";
			//float before = r->GetTotalReward();
			a = r->Update();
			//float after = r->GetTotalReward();
			//rewards.push_back(after - before);			
		}
		cout << "\n";
		if (RL_TYPE == 2)
		{
			//cout << "I am building a LAB\n";
			//float oldFrame = g->frame;
			g->ConstructBuilding(RL_LAB_ID);
			//cout << "Frame duration: " << g->frame - oldFrame << "\n";
			a = r->Update();
		} 
		
		//if(r->GetTotalReward() > bestReward)
		//{
		//	bestReward = r->GetTotalReward();
		//	for(int j = 0; j < actions.size(); j++)
		//	{		
		//		cout << "resources solar" << j << ": " << resource1[j] << "\n";
		//		cout << "resources mex" << j << ": " << resource2[j] << "\n";
		//		cout << "action" << j << ": " << actions[j].Action << "\n";	
		//		cout << "reward for this: " << rewards[j] << " J: " << j << "\n\n";
		//	}
		//	cout << "Totalreward: " << r->GetTotalReward() << "\n\n" ;
		//}

		cout << i << "\t" << r->GetTotalReward() << "\n" ;

		//if (i%50==0) cout << i << "\t" << r->GetTotalReward() << "\n" ;

		g->ResetGame();
		delete r;

		//if(i%100 == 0)
			//cout << "I:" << i << "\n";
		i++;
	}
	//system("pause");
}

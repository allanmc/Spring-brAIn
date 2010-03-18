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
#ifdef WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);  // Get handle to standard output
    SetConsoleTextAttribute(hConsole,theColour);  // set the text attribute of the previous handle
#endif
}


void PrintAction(bool debug,RL_Action a, unsigned short builder)
{
	if (debug)
	{
		int action;

		action = a.Action;

		switch(action)
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
			case NOTHING_ID:
				cerr << "N";
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
	int numLearners = NUM_LEARNERS;

	bool debug = false;

	cout << (USE_QSMDP ? "SMDPQ" : "Q");

	if (EPSILON_DECAY!=1)
	{
		cout << ", decay=" << EPSILON_DECAY;
	}

	cout << " ";

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
	int runs = RUNS_TO_DO;
	if (TEST_RESULTS)
		runs = 1;
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
		
		for ( unsigned int cTerm = 0 ; cTerm < RL_LAB_INDEX ; cTerm++ )
		{
			if (TEST_RESULTS)
			{
				cout << endl << endl << " - Building lab " << cTerm << ":" << endl;
				cout << "Metal Res: " << g->resources[MEX_ID] << endl;
				cout << "Energy Res: " << g->resources[SOLAR_ID] << endl;
				cout << "Metal Gain: " << g->GetTotalProduction(MEX_ID) - g->GetResourceUsage(MEX_ID) << endl;
				cout << "Energy Gain: " << g->GetTotalProduction(SOLAR_ID) - g->GetResourceUsage(SOLAR_ID) << endl;
			}

			r = new RL(g, currentEpsilon, numLearners, i==0);

			if ( i == 0 && cTerm == 0 ) //If first run, initialize visit statistics 
			{
				numStates = r->numStates;
				numActions = r->numActions;
				InitStateVisits();
			}

			RL_State::lastLabCount = g->units[LAB_ID];
			//Delete old Q-file?
			if ( i == 0 && RL_FILE_DELETE && !TEST_RESULTS)
			{
				char* path = r->GetFilePath();
				if(remove(path) != 0)
					perror("deletion fail: ");
				delete[] path;
			}
			
			RL_Action a;
			for(int x = 0; x < numLearners; x++)
			{
				a = r->Update(x);
				UpdateStateVisits(r);
				PrintAction(debug, a, x);
				
				g->BuildUnit(a.Action, x);
			}

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
					UpdateStateVisits(r);
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
			if (TEST_RESULTS)
			{
				cout << endl << "Reward ============== " << r->GetTotalReward() << endl;
				cout << "Metal Res: " << g->resources[MEX_ID] << endl;
				cout << "Energy Res: " << g->resources[SOLAR_ID] << endl;
				cout << "Metal Gain: " << g->GetTotalProduction(MEX_ID) - g->GetResourceUsage(MEX_ID) << endl;
				cout << "Energy Gain: " << g->GetTotalProduction(SOLAR_ID) - g->GetResourceUsage(SOLAR_ID) << endl;
			}
			currentReward += r->GetTotalReward();
			delete r;
		}

		if ( debug && !TEST_RESULTS )
		{
			cerr << endl;
			cout << "Metal Res: " << g->resources[MEX_ID] << endl;
			cout << "Energy Res: " << g->resources[SOLAR_ID] << endl;
			cout << "Metal Gain: " << g->GetTotalProduction(MEX_ID) - g->GetResourceUsage(MEX_ID) << endl;
			cout << "Energy Gain: " << g->GetTotalProduction(SOLAR_ID) - g->GetResourceUsage(SOLAR_ID) << endl;
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
		
		if (!TEST_RESULTS) {
			if(PRINT_REWARD)
			{
				cout << currentReward << "\n";
			}
			else
			{
				cout << currentFrame << (currentFrame == bestFrame && debug ? "*" : "") << "\n" ;
			}
		}
		
		currentIndex++;

		if(goodNew)
		{
			bestFrame = currentFrame;
			bestReward = currentReward;
			if (debug && !TEST_RESULTS) system("pause");
		}

		currentReward = 0.0;
		currentFrame = 0.0;
		ChangeColour(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); 

		if(runs > 100 && i%(runs/100) == 0 && !TEST_RESULTS)
			cerr << "Status:" << floor(((i/(double)runs)*100)+0.5) << "%\xd";
		i++;

		currentEpsilon *= EPSILON_DECAY;
	}
	SaveStateVisits();
	//save to file
	r = new RL(g, currentEpsilon, numLearners, false);
	r->SaveToFile(true);
	delete r;
	//end save

	//cerr << endl << "End epsilon: " << currentEpsilon << endl;
}

void InitStateVisits()
{
	actionStateVisits = new unsigned int[numActions*numStates];
	stateVisits = new unsigned int[numStates];
	for ( int i = 0 ; i < numActions*numStates ; i++ )
		actionStateVisits[i] = 0;
	for ( int i = 0 ; i < numStates ; i++ )
		stateVisits[i] = 0;
}

void UpdateStateVisits(RL *r)
{
	stateVisits[r->LastStateID]++;
	actionStateVisits[r->LastActionID + numActions * r->LastStateID]++;
}

void SaveStateVisits()
{
	ofstream *file;
	char *path = new char[200];
	strcpy(path, RL_FILE_PATH);
	strcat(path, RL_FILE_ACTIONSTATEVISITS);
	file = new ofstream(path, ios::binary | ios::out);
	file->write( (char*)actionStateVisits, sizeof(unsigned int)*numStates*numActions );
	file->flush();
	file->close();
	delete file;

	
	delete[] path;
	path = new char[200];
	strcpy(path, RL_FILE_PATH);
	strcat(path, RL_FILE_STATEVISITS);
	file = new ofstream(path, ios::binary | ios::out);
	file->write( (char*)stateVisits, sizeof(unsigned int)*numStates );
	file->flush();
	file->close();
	delete file;

	delete[] path;
}
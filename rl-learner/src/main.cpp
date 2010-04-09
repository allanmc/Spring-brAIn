#include "main.h"

using namespace std;
using namespace brainSpace;

int main(int argc, char *argv[])
{
	LoadConfig(argc, argv);
	cerr << EPSILON_DECAY << endl;
	PrintOutputHeader();
	srand((unsigned int)time(NULL));

	//Delete old Q-file?
	if (RL_FILE_DELETE && !TEST_RESULTS)
	{
		char* path = RL::GetFilePath();
		if(remove(path) != 0)
			perror("deletion fail: ");
		delete[] path;
	}

	Game *g = new Game();
	RL *r;

	bool debug = false;

	double currentEpsilon = EPSILON_START;
	double currentReward = 0.0;
	double currentFrame = 0.0;
	double bestFrame = 999999;
	double bestReward = -999999;
	int currentIndex = 0;

	if (TEST_RESULTS)
		RUNS = 1;

	for(int run = 0; run < RUNS; run++)
	{
		g_currentGame = run;
		//if ( run % 500 == 0 )
		//	cerr << "Eps: " << currentEpsilon << endl;
		if ( run == RUNS-1  )
		{
			debug = true;
			cerr << "Eps: " << currentEpsilon << endl;
			currentEpsilon = 0.0f;
		}
#ifdef USE_NEW_REWARD_CODE
		unsigned int numTerm = 1;
#else
		unsigned int numTerm = RL_LAB_INDEX; //1
#endif
		for ( unsigned int cTerm = 0 ; cTerm < numTerm ; cTerm++ )
		{
			if (TEST_RESULTS)
			{
				cout << endl << endl << " - Building lab " << cTerm << ":" << endl;
				PrintGameStatus(g);
			}

			r = new RL(g, currentEpsilon, NUM_LEARNERS, run==0);

			if ( run == 0 && cTerm == 0 ) //If first run, initialize visit statistics 
			{
				numStates = r->numStates;
				numActions = r->numActions;
				InitStateVisits();
			}

			RL_State::lastLabCount = g->units[LAB_ID];
			bool* isBuildingLab = new bool[NUM_LEARNERS];
			RL_Action a;
			for(int x = 0; x < NUM_LEARNERS; x++)
			{
				a = r->Update(x);
				if(a.Action == LAB_ID)
				{
					isBuildingLab[x] = true;
				}
				else
				{
					isBuildingLab[x] = false;
				}
				UpdateStateVisits(r);
				PrintAction(debug, a, x);
				
				g->BuildUnit(a.Action, x);
			}

			int runningAgents = NUM_LEARNERS;
			while( runningAgents > 0 )
			{
				vector<int> builders;
				while(true)
				{
					//game loop
					builders = g->Update();
					if(!builders.empty())
					{
						break;
					}
				}
				if (builders.size() > 1) {
					int i = 0;
				}
				for(int i = 0; i < (int)builders.size(); i++)
				{
					bool print = false;
					if(debug && isBuildingLab[builders[i]])
					{
						isBuildingLab[builders[i]] = false;
						//print = true;
					}
					a = r->Update(builders[i]);
					if(print)
					{
						cerr << "LastReward: " << r->GetLastReward() << " ";
						cerr << "StateID: " << r->LastStateID << " " << endl;
					}
					UpdateStateVisits(r);
					if ( a.ID != -1 ) 
					{
						g->BuildUnit(a.Action, builders[i]);
						PrintAction(debug, a, builders[i]);
						if(a.Action == LAB_ID)
						{
							isBuildingLab[builders[i]] = true;
						}
					}
					else
					{
						if (debug) 
						{
							cerr << "LastReward: " << r->GetLastReward() << " ";
							cerr << "StateID: " << r->LastStateID << " " << endl;
							cerr << "T" << builders[i] << " ";
						}
						runningAgents--;
					}
				}
			}
			if (TEST_RESULTS)
			{
				cout << endl << "Reward ============== " << r->GetTotalReward() << endl;
				PrintGameStatus(g);
			}

			currentReward += r->GetTotalReward() / NUM_LEARNERS;

			delete r;
		}

		if ( debug && !TEST_RESULTS )
		{
			cerr << endl;
			PrintGameStatus(g);
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
			//if (debug && !TEST_RESULTS) system("pause");
		}

		currentReward = 0.0;
		currentFrame = 0.0;
		ChangeColour(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE); 

		if(RUNS > 100 && run%(RUNS/100) == 0 && !TEST_RESULTS)
			cerr << "Status:" << floor(((run/(double)RUNS)*100)+0.5) << "%\xd";

		currentEpsilon *= EPSILON_DECAY;
	}

	PrintStateVisits();
	SaveStateVisits();
	//save to file
	r = new RL(g, currentEpsilon, NUM_LEARNERS, false);
	r->SaveToFile(true);
	delete r;
	//end save

	//cerr << endl << "End epsilon: " << currentEpsilon << endl;
}

void ChangeColour(WORD theColour)
{
#ifdef WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);  // Get handle to standard output
    SetConsoleTextAttribute(hConsole,theColour);  // set the text attribute of the previous handle
#endif
}

void PrintGameStatus(Game *g)
{
	cerr << "Metal Res: " << g->resources[MEX_ID] << endl;
	cerr << "Energy Res: " << g->resources[SOLAR_ID] << endl;
	cerr << "Metal Gain: " << g->GetTotalProduction(MEX_ID) - g->GetResourceUsage(MEX_ID) - g->units[LAB_ID]*5 << endl;
	cerr << "Energy Gain: " << g->GetTotalProduction(SOLAR_ID) - g->GetResourceUsage(SOLAR_ID) - g->units[LAB_ID]*50 << endl;
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

void PrintOutputHeader()
{
	cout << "x" << NUM_LEARNERS << " ";

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
}

void InitStateVisits()
{
	actionStateVisits = new unsigned int[numActions*numStates];
	stateVisits = new unsigned int[numStates];
	for ( unsigned int i = 0 ; i < numActions*numStates ; i++ )
	{
		actionStateVisits[i] = 0;
	}
	for ( unsigned int i = 0 ; i < numStates ; i++ )
	{
		stateVisits[i] = 0;
	}
}

void UpdateStateVisits(RL *r)
{
	stateVisits[r->LastStateID]++;
	actionStateVisits[r->LastActionID + numActions * r->LastStateID]++;
}


void PrintStateVisits()
{
	unsigned int unique_visits = 0;
	unsigned int total_visits = 0;

	for ( unsigned int i = 0; i < numStates ; i++ )
	{
		if (stateVisits[i]>0)
		{
			unique_visits++;
			total_visits += stateVisits[i];
		}
	}
	cerr << "Unique State Visits = " << unique_visits << endl;


	unique_visits = 0;
	for ( unsigned int i = 0; i < numStates*numActions ; i++ )
	{
		if (actionStateVisits[i]>0)
		{
			unique_visits++;
		}
	}
	cerr << "Unique State-Action Visits = " << unique_visits << endl;
	cerr << "Total Visits = " << total_visits << endl;

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

void LoadConfig(int argc, char *argv[])
{
	//load default values
	RUNS = 100000;
	NUM_LEARNERS = 3;
	GAMMA = 0.9f;
	ALPHA = 0.1f;
	EPSILON_START = 0.5f;
	EPSILON_DECAY = 0.99999f;

	PRINT_REWARD  = true;

	USE_QSMDP = true;
	USE_RS_TERMINATION = true;
	USE_RS_TIME = false;
	USE_Q_LAMBDA = false;

	try {
		ptree pt;
		read_info("config", pt);
		RUNS = pt.get("runs", RUNS);
		NUM_LEARNERS = pt.get("num_learners", NUM_LEARNERS);
		GAMMA = pt.get("gamma", GAMMA);
		ALPHA = pt.get("alpha", ALPHA);
		EPSILON_START = pt.get("epsilon_start", EPSILON_START);
		EPSILON_DECAY = pt.get("epsilon_decay", EPSILON_DECAY);
		double epsilon_end = pt.get("epsilon_end", 0.0);
		if(epsilon_end != 0.0)
		{
			double first = (double)epsilon_end/(double)EPSILON_START;
			double second = 1.0/(double)RUNS;			
			EPSILON_DECAY = pow(first, second);
		}
		PRINT_REWARD = pt.get("print_reward", PRINT_REWARD);
		USE_QSMDP = pt.get("use_qsmdp", USE_QSMDP);
		USE_RS_TERMINATION = pt.get("use_rs_termination", USE_RS_TERMINATION);
		USE_RS_TIME = pt.get("use_rs_time", USE_RS_TIME);
		USE_Q_LAMBDA = pt.get("use_q_lambda", USE_Q_LAMBDA);
	}
    catch(exception& e) {
		cerr << "Coult not load config: " << e.what() << "\n";
    }
	try {
		po::options_description desc("Allowed options");
		desc.add_options()
			("help,h", "Help me!")
			("runs,n", po::value<int>(), "Number of runs to do")
			("num_learners,l", po::value<int>(), "Number of learners")
			("gamma,g", po::value<float>(), "Gamma value")
			("alpha,a", po::value<float>(), "Alpha value")
			("epsilon_start,e", po::value<double>(), "Starting epsilon value")
			("epsilon_decay,d", po::value<double>(), "Epsilon-decay value")
			("epsilon_end,f", po::value<double>(), "Epsilon-end value(overwrites decay)")
			("print_reward,r", po::value<bool>(), "Print reward? Else time is pritned.")
			("use_smdpq,s", po::value<bool>(), "Use smdpq?")
			("use_rs_termination,t", po::value<bool>(), "Use special termination reward?")
			("use_rs_time,m", po::value<bool>(), "Use TRS?")
			("use_q_lambda,y", po::value<bool>(), "Use Q-lambda?")
		;

		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);    

		if (vm.count("help")) {
			cout << desc << "\n";
			exit(0);
		}
		if (vm.count("runs")) {
			RUNS = vm["runs"].as<int>();
		}
		if (vm.count("num_learners")) {
			NUM_LEARNERS = vm["num_learners"].as<int>();
		}
		if (vm.count("gamma")) {
			GAMMA = vm["gamma"].as<float>();
		}
		if (vm.count("alpha")) {
			ALPHA = vm["alpha"].as<float>();
		}
		if (vm.count("epsilon_start")) {
			EPSILON_START = vm["epsilon_start"].as<double>();
		}
		if (vm.count("epsilon_decay")) {
			EPSILON_DECAY = vm["epsilon_decay"].as<double>();
		}
		if (vm.count("epsilon_end")) {
			double epsilon_end = vm["epsilon_end"].as<double>();
			EPSILON_DECAY = pow((double)epsilon_end/(double)EPSILON_START, 1.0/(double)RUNS);
		}
		if (vm.count("print_reward")) {
			PRINT_REWARD = vm["print_reward"].as<bool>();
		}
		if (vm.count("use_qsmdp")) {
			USE_QSMDP = vm["use_qsmdp"].as<bool>();
		}
		if (vm.count("use_rs_termination")) {
			USE_RS_TERMINATION = vm["use_rs_termination"].as<bool>();
		}
		if (vm.count("use_rs_time")) {
			USE_RS_TIME = vm["use_rs_time"].as<bool>();
		}
		if (vm.count("use_q_lambda")) {
			USE_Q_LAMBDA = vm["use_q_lambda"].as<bool>();
		}

	}
    catch(exception& e) {
        cerr << "Could not get command options: " << e.what() << "\n";
    }
}

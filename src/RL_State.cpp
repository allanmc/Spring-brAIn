#include "RL_State.h"
#include "RL.h"
#include "Battle.h"
#include <map>
#include "Knowledge.h"
using namespace brainSpace;


RL_State::RL_State()
{
}

RL_State::RL_State(AIClasses* aiClasses, MilitaryUnitGroup* group, std::vector<QStateVar> stateVars, vector<pair<int, SAIFloat3> > resourceBuildings, RL_Q* valueFunction, double epsilon, int type )
{
	ai = aiClasses;
	Type = type;
	//Reader = new BattleFileReader(ai);
	vector< vector< pair< int, SAIFloat3> > > groups(3);
	ExpectedReward = -10000000;

	//This one holds the real building clusters
	//Even though two buildings are inside a short range of the group, does not mean they should belong to the same cluster
	vector< vector<pair<int, SAIFloat3> > > realClusters;

	for ( unsigned int i = 0 ; i < resourceBuildings.size() ; i++ )
	{
		double distance = ai->utility->EuclideanDistance( resourceBuildings[i].second, group->GetPos() );
		if ( distance < 650 )
		{
			groups[0].push_back( resourceBuildings[i] );
			//ai->utility->ChatMsg("RL_STATE: Group DistInterval 0 new size %d", groups[0].size() );
		}
		else if ( distance < 1300 )
		{
			groups[1].push_back( resourceBuildings[i] );
			//ai->utility->ChatMsg("RL_STATE: Group DistInterval 1 new size %d", groups[1].size() );
		}
		else
		{
			groups[2].push_back( resourceBuildings[i] );
			//ai->utility->ChatMsg("RL_STATE: Group DistInterval 2 new size %d", groups[2].size() );
		}
	}

	//Building-independent statevars
	float GroupSpeed = 0.0f;
	vector<int> uIDs = group->GetUnits();	
	for ( int i = 0 ; i < group->GetSize() ; i++ )
	{
		Unit* u = Unit::GetInstance(ai->callback, uIDs[i] );
		if ( u == NULL )
		{
			ai->utility->ChatMsg("RL_State: NULL UNIT");
		}
		UnitDef* d = u->GetDef();
		if ( d == NULL )
		{
			ai->utility->ChatMsg("RL_State: NULL DEF");
			delete d;
			d = NULL;
			delete u;
			u = NULL;
			return;
		}
		GroupSpeed += d->GetSpeed();
		delete d;
		d = NULL;
		delete u;
		u = NULL;
	}
	GroupSpeed /= group->GetSize();


	for ( unsigned int i = 0 ; i < groups.size() ; i++ )
	{
		if ( groups[i].empty() )
		{
		//	ai->utility->ChatMsg("Resource type %d group %d empty", type, i );
			continue;
		}
		//ai->utility->ChatMsg("Resource type %d group %d not empty. Size %d", type, i, groups[i].size() );
		for ( unsigned int j = 0 ; j < groups[i].size() ; j++ )
		{
		//	ai->utility->ChatMsg("Iterating unit %d in group %d", j, i );
			bool buildingGood = true;
			bool inserted = false;
			for ( unsigned int k = 0 ; k < realClusters.size() ; k++ )
			{
			//	ai->utility->ChatMsg("Iter clusters. Numclusters: %d", realClusters.size() );
				for ( unsigned int l = 0 ; l < realClusters[k].size() ; l++ )
				{
				//	ai->utility->ChatMsg("CLuster %d size: %d", k, realClusters[k].size() );
					double dist = ai->utility->EuclideanDistance( groups[i][j].second, realClusters[k][l].second );
				//	ai->utility->ChatMsg("Distance between (%f,%f) - (%f,%f): %f", groups[i][j].second.x, groups[i][j].second.z, realClusters[k][l].second.x, realClusters[k][l].second.z, dist );
					//ai->utility->ChatMsg("RL_STATE: Dist %f between group[%d][%d] and clustermex[%d][%d]", dist, i, j, k, l );
					if ( dist > RESOURCE_BUILDING_GROUP_TOLERANCE )
					{
						/*
						The building specified by groups[i][j] was too far away from 
						a building in the cluster specified by realClusters[k][l]
						*/
						buildingGood = false;
						//ai->utility->ChatMsg("RL_STATE: MexSpot was false");
						break;//no need to iterate the rest of this cluster, so move to the next one
					}
				}
				if ( buildingGood )
				{
				//	ai->utility->ChatMsg("Can insert building in cluster %d", k );
					realClusters[k].push_back( groups[i][j] );
					inserted = true;
				//	ai->utility->ChatMsg("New cluster size for %d: %d", k, realClusters[k].size() );
				}
			}
			//We have now iterated all the currently created clusters

			/*
			This means that the building was too far away from any existing buildings, or no clusters yet exist.
			In any case, create a new cluster
			*/
			if ( !inserted )
			{
				//ai->utility->ChatMsg("Starting a new cluster" );
				realClusters.push_back( vector<pair<int, SAIFloat3> >() );
				realClusters.back().push_back( groups[i][j] );
			}
		}
	}

	/*
	So we finally grouped all buildings in the specified distance from the current militarygroup
	We now need to figure out what is the best cluster to consider for RL
	*/
	float optimalClusterReward = -10000000.0f;
	int optimalStateID = -1;
	vector<int> optimalUnitIDs;
	Path* optimalPath = NULL;


	//StateID, pair<Action, ExpectedReward > >
	map<unsigned int, pair< RL_Action*, double > > possibleStates;


	//Iterate through clusters
	//ai->utility->ChatMsg("RL_STATE: %d clusters", realClusters.size());
	for ( unsigned int j = 0 ; j < realClusters.size() ; j++ )
	{
		//ai->utility->ChatMsg("Making state. Cluster %d. Size %d", j, realClusters[j].size() );
		float DistBuildingSpot = 0.0f;
		Superiority BuildingSpotImaginaryInf, CurrentSpotInf;
		//int SuperiorPathLength = -1;

		vector<int> unitIDs;

		SAIFloat3 average;
		average.x = 0.0f;
		average.y = 0.0f;
		average.z = 0.0f;

		double closestDist = 100000;
		int closestIndex = -1;

		//Iterate through members of this cluster
		for ( unsigned int k = 0 ; k < realClusters[j].size() ; k++ )
		{
			//ai->utility->ChatMsg("Finding position: (%f,%f,%f)", realClusters[j][k].second.x, realClusters[j][k].second.y, realClusters[j][k].second.z );
			average.x += realClusters[j][k].second.x;
			average.y += realClusters[j][k].second.y;
			average.z += realClusters[j][k].second.z;
			unitIDs.push_back(realClusters[j][k].first );
			double dist = ai->utility->EuclideanDistance( realClusters[j][k].second, group->GetPos() );
			if ( dist < closestDist )
			{
				closestDist = dist;
				closestIndex = k;
			}
		}
		average.x /= realClusters[j].size();
		average.y /= realClusters[j].size();
		average.z /= realClusters[j].size();

		DistBuildingSpot = ai->utility->EuclideanDistance( average, group->GetPos() );

		BuildingSpotImaginaryInf = ai->knowledge->mapInfo->threatMap->GetImaginarySuperiorityAtPos( average, group, false );

		CurrentSpotInf = ai->knowledge->mapInfo->threatMap->GetSuperiorityAtPos( group->GetPos() );
		//ai->utility->ChatMsg("RL_STATE: Imaginary inf at mexspot (%f,%f) %f", average.x, average.z, (float)MexSpotImaginaryInf);
		
		
		Path* p = NULL;
		if ( BuildingSpotImaginaryInf == INFERIOR || BuildingSpotImaginaryInf == EQUAL )
		{
			//SuperiorPathLength = 0;
		}
		else
		{
			p = ai->knowledge->mapInfo->pathfindingMap->FindSuperiorPathTo( group, group->GetPos(), average, true );
			//SuperiorPathLength = (p == NULL || p->GetLength() == 0 ) ? 0 : 1;
		}

		//Calculate the ID!
		ID = 0;
		unsigned int offset = 1;
		//ai->utility->ChatMsg("STATE SIZE: %d", stateVars.size() );
		for ( int m = (int)stateVars.size()-1 ; m >= 0 ; m-- )
		{
			unsigned int current = 0;
			switch( m )
			{
			case 0: current = ( realClusters[j].size() < 2 ) ? 0 : ( realClusters[j].size() < 3 ) ? 1 : 2 ; break;
			case 1: current = ( DistBuildingSpot < 650 ) ? 0 : ( DistBuildingSpot < 1300 ) ? 1 : 2; break;
			//case 2: current = MexSpotInf; break;
			case 2: current = BuildingSpotImaginaryInf; break;
			//case 3: current = AirGroup; break;
				//Slow = kbots, medium = tanks and slow aircrafts, fast = fighters, bombers
			case 3: current = ( GroupSpeed < 70 ) ? 0 : ( GroupSpeed < 190 ) ? 1 : 2; break;
			//case 4: current = SuperiorPathLength; break;
			case 4: current = CurrentSpotInf; break;
			//default: ai->utility->ChatMsg("RL_State: Unexpected Statevar!");
			}
				
			ID += current*offset;
			offset *= stateVars[m].numStates;
			//ai->utility->ChatMsg("StateVar: %d. ID: %d. Current: %d", m, ID, current );
		}
		//ai->utility->ChatMsg("RL_State: ID %d", ID );
		if ( BuildingSpotImaginaryInf == SUPERIOR || BuildingSpotImaginaryInf == EQUAL )
		{
			//ai->utility->ChatMsg("ID: %d. ImgInf: %d", ID, BuildingSpotImaginaryInf );
		}
		if ( CurrentSpotInf == SUPERIOR || CurrentSpotInf == EQUAL )
		{
			//ai->utility->ChatMsg("ID: %d. CurInf: %d", ID, CurrentSpotInf );
		}
		terminal = true;
		RL_Action* a = new RL_Action( ai, 0, group, unitIDs, p );
		double val = valueFunction->GetValue( ID, *a );
		possibleStates.insert( make_pair( ID, make_pair( a, val ) ) );
	}


	//ai->utility->ChatMsg("RL_STATE: State alternatives type %d: %d", type, possibleStates.size() );
	if ( possibleStates.size() > 0 )
	{
		
		/*
		//EPSILON-GREEDY STATE CHOICE
		float r = rand()/(float)RAND_MAX;
		if ( r <= epsilon ) //non-greedy
		{
			int index = rand()%possibleStates.size();
			ai->utility->ChatMsg("Non-greedy state choice: %d out of %d", index, possibleStates.size() );
			map<unsigned int, pair<RL_Action*, double> >::iterator it = possibleStates.begin();
			for ( int i = 0 ; i < index ; i++ )
				it++;

			optimalClusterReward = it->second.second;
			optimalPath = it->second.first->Path;
			optimalStateID = it->first;
			optimalUnitIDs = it->second.first->unitIDs;
		}
		else
		{
			for ( map<unsigned int, pair<RL_Action*, double> >::iterator it = possibleStates.begin() ; it != possibleStates.end() ; it++ )
			{
				if ( it->second.second > optimalClusterReward )
				{
					optimalPath = it->second.first->Path;
					optimalStateID = it->first;
					optimalUnitIDs = it->second.first->unitIDs;
					optimalClusterReward = it->second.second;
				}
			}
		}
		ID = optimalStateID;
		ExpectedReward = optimalClusterReward;
		Actions.push_back( possibleStates.find(ID)->second.first );
		*/
		
		int fewestVisits = 1000000;
		for ( map<unsigned int, pair<RL_Action*, double> >::iterator it = possibleStates.begin() ; it != possibleStates.end() ; it++ )
		{
			int v = GetVisitsTo(it->first, type );
			if ( v < fewestVisits )
			{
				fewestVisits = v;
				optimalStateID = it->first;
				optimalClusterReward = it->second.second;
				optimalPath = it->second.first->Path;
				optimalUnitIDs = it->second.first->unitIDs;
			}
		}

		ID = optimalStateID;
		ExpectedReward = optimalClusterReward;
		Actions.push_back( possibleStates.find(ID)->second.first );
	}
	else
	{
		//The current environment could not be represented in a valid state
		ID = -1;
		ExpectedReward = 0;
	}
	map<unsigned int, pair<RL_Action*, double> >::iterator it = possibleStates.begin() ;
	while(  it != possibleStates.end() )
	{
		if ( it->first != ID )
		{
			int a = it->first;
			delete it->second.first;
			it->second.first = NULL;
			it++;
			possibleStates.erase( a );
		}
		else
			it++;
	}
}

RL_State::~RL_State()
{
	Actions.clear();
}

int RL_State::GetID()
{
	return ID;
}

std::vector<RL_Action*> RL_State::GetActions()
{
	return Actions;
}

void RL_State::DeleteAction(int actionID)
{
	for (unsigned int i = 0; i < Actions.size(); i++)
	{
		if(Actions[i]->Action == actionID)
		{
			//delete Actions[i];
			//Actions[i] = NULL;
			Actions.erase(Actions.begin()+i);
			return;
		}
	}
}

bool RL_State::IsTerminal()
{
	return terminal;
}

bool RL_State::operator==(const RL_State &other) const
{
	return (ID == other.ID);
}

RL_State & RL_State::operator=(const RL_State &rhs)
{
	if (this == &rhs) // Same object?
		return *this;// Yes, so skip assignment, and just return *this.

	Actions = rhs.Actions;
	ID = rhs.ID;
	terminal = rhs.terminal;
	return *this;
}

int RL_State::UpdateVisitsTo(int stateID, int type)
{
	fstream *file = new fstream();
	char* path = GetFilePath(type);
	file->open(path, ios::in | ios::binary );

	//file did not exist!!
	if ( !file->is_open() )
	{
		ai->utility->ChatMsg("UpdateStateVisits: FILE NOT OPEN");
		file->close();
		delete file;
		file = NULL;
		delete path;
		path = NULL;
		return 0;
	}
	file->close();
	file->open( path, ios::out | ios::binary | ios::in );
	int v = ai->utility->ReadFromStateVisitFile( file, stateID );
	ai->utility->WriteToStateVisitFile( file, v+1, stateID );
	file->close();
	delete file;
	file = NULL;
	delete[] path;
	path = NULL;
	return v;
}

int RL_State::GetVisitsTo(int stateID, int type)
{
	fstream *file = new fstream();
	char* path = GetFilePath(type);
	file->open(path, ios::in | ios::binary );

	//file did not exist!!
	if ( !file->is_open() )
	{
		ai->utility->ChatMsg("FILE NOT OPEN");
		file->close();
		file->open( path, ios::binary | ios::out );

		long a = file->tellp();
		ai->utility->ChatMsg( "p start: %d", a);

		for ( unsigned int i = 0 ; i < NUM_STATE_RESOURCES_SUBNODES ; i++ )
		{
			ai->utility->WriteToStateVisitFile( file, 0 );
		}
		a = file->tellp();
		ai->utility->ChatMsg( "p end: %d", a);

		file->close();
		delete file;
		file = NULL;
		delete[] path;
		path = NULL;
		return 0;
	}
	file->close();
	file->open( path, ios::out | ios::binary | ios::in );
	int v = ai->utility->ReadFromStateVisitFile( file, stateID );
	file->close();
	delete file;
	file = NULL;
	delete path;
	path = NULL;
	return v;
}


char* RL_State::GetFilePath(int type)
{
	DataDirs *dirs = ai->callback->GetDataDirs();
	const char* dir = dirs->GetWriteableDir();

	char *path = new char[200];
	strcpy(path, dir);
	switch( type )
	{
	case 0:
		strcat( path, RL_FILE_ATTACK_MEX_VISITS );
		break;
	case 1:
		strcat( path, RL_FILE_ATTACK_SOL_VISITS );
		break;
	case 2:
		strcat( path, RL_FILE_ATTACK_WIN_VISITS );
		break;
	}
	delete dirs;
	dirs = NULL;
	return path;
}
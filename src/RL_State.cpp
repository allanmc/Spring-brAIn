#include "RL_State.h"
#include "RL.h"
#include "Battle.h"
#include <map>
#include "Knowledge.h"
using namespace brainSpace;


RL_State::RL_State()
{
	Reader = NULL;
}

RL_State::RL_State(AIClasses* aiClasses, MilitaryUnitGroup* group, std::vector<QStateVar> stateVars, vector<pair<int, SAIFloat3> > mexCluster, RL_Q* valueFunction )
{
	ai = aiClasses;
	//ai->utility->ChatMsg("RL_State Started!!!!");
	Reader = new BattleFileReader(ai);
	vector< vector< pair< int, SAIFloat3> > > groups(4);

	//This one holds the real mex clusters
	//Even though two mexes are inside a short range of the group, does not mean they should belong to the same cluster
	vector< vector<pair<int, SAIFloat3> > > realClusters;



	//ai->utility->ChatMsg("RL_STATE: MexPositions: %d", mexCluster.size() );
	for ( unsigned int i = 0 ; i < mexCluster.size() ; i++ )
	{
		double distance = ai->utility->EuclideanDistance( mexCluster[i].second, group->GetPos() );
		if ( distance < 400 )
		{
			groups[0].push_back( mexCluster[i] );
			//ai->utility->ChatMsg("RL_STATE: Group DistInterval 0 new size %d", groups[0].size() );
		}
		else if ( distance < 1000 )
		{
			groups[1].push_back( mexCluster[i] );
			//ai->utility->ChatMsg("RL_STATE: Group DistInterval 1 new size %d", groups[1].size() );
		}
		else if ( distance < 2000 )
		{
			groups[2].push_back( mexCluster[i] );
			//ai->utility->ChatMsg("RL_STATE: Group DistInterval 2 new size %d", groups[2].size() );
		}
		else
		{
			groups[3].push_back( mexCluster[i] );
			//ai->utility->ChatMsg("RL_STATE: Group DistInterval 3 new size %d", groups[3].size() );
		}
	}

	//Mex-independent statevars
	bool AirGroup = group->IsAirGroup();
	float GroupSpeed = 0.0f;
	vector<int> uIDs = group->GetUnits();	
	for ( unsigned int i = 0 ; i < group->GetSize() ; i++ )
	{
		Unit* u = Unit::GetInstance(ai->callback, uIDs[i] );
		UnitDef* d = u->GetDef();
		GroupSpeed += d->GetSpeed();
		delete d;
		delete u;
	}
	GroupSpeed /= group->GetSize();


	//4 = the number of intervals in the distance state variable
	for ( unsigned int i = 0 ; i < 4 ; i++ )
	{
		if ( groups[i].empty() )
		{
			continue;
		}
		for ( unsigned int j = 0 ; j < groups[i].size() ; j++ )
		{
			//ai->utility->ChatMsg("RL_STATE: Group %d not empty. Size %d", i, groups[i].size() );
			bool mexGood = true;
			bool inserted = false;
			for ( unsigned int k = 0 ; k < realClusters.size() ; k++ )
			{
				//ai->utility->ChatMsg("RL_STATE: A cluster found");
				for ( unsigned int l = 0 ; l < realClusters[k].size() ; l++ )
				{
					double dist = ai->utility->EuclideanDistance( groups[i][j].second, realClusters[k][l].second );
					//ai->utility->ChatMsg("RL_STATE: Dist %f between group[%d][%d] and clustermex[%d][%d]", dist, i, j, k, l );
					if ( dist > MEX_GROUP_TOLERANCE )
					{
						/*
						The mex specified by groups[i][j] was too far away from 
						one of the mexes in the cluster specified by realClusters[k]
						*/
						mexGood = false;
						//ai->utility->ChatMsg("RL_STATE: MexSpot was false");
						break;//no need to iterate the rest of this cluster, so move to the next one
					}
				}
				if ( mexGood )
				{
					realClusters[k].push_back( groups[i][j] );
					inserted = true;
					//ai->utility->ChatMsg("RL_STATE: MexSpot was good");
				}
			}
			//We have now iterated all the currently created clusters

			/*
			This means that the mex was too far away from any existing mexes, or no clusters yet exist.
			In any case, create a new cluster
			*/
			if ( !inserted )
			{
				//ai->utility->ChatMsg("RL_STATE: All clusters iterated. Still not inserted");
				realClusters.push_back( vector<pair<int, SAIFloat3> >() );
				realClusters.back().push_back( groups[i][j] );
				//ai->utility->ChatMsg("RL_STATE: Mex inserted in cluster %d. New size: %d", realClusters.size(), realClusters.back().size() );
			}
		}
	}

	/*
	So we finally grouped all mexes in the specified distance from the current militarygroup
	We now need to figure out what is the best cluster to consider for RL
	*/
	int optimalClusterIndex = -1;
	float optimalClusterReward = -10000000.0f;
	int optimalStateID = -1;
	vector<int> optimalUnitIDs;
	Path* optimalPath = NULL;


	//StateID, pair<Action, ExpectedReward > >
	map<int, pair< RL_Action*, double > > possibleStates;


	//Iterate through clusters
	//ai->utility->ChatMsg("RL_STATE: %d clusters", realClusters.size());
	for ( unsigned int j = 0 ; j < realClusters.size() ; j++ )
	{
		unsigned int MexCount = realClusters[j].size();
		float DistMexSpot = 0.0f;
		Superiority MexSpotInf, MexSpotImaginaryInf, CurrentSpotInf;
		int SuperiorPathLength = -1;

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

		DistMexSpot = ai->utility->EuclideanDistance( average, group->GetPos() );

		//ai->utility->ChatMsg("RL_STATE: MexCount %d. MexDist: %f. MexSpot: (%f,%f). Group (%f,%f)", MexCount, DistMexSpot, average.x, average.z, group->GetPos().x, group->GetPos().z );
		
		//MexSpotInf = ai->knowledge->mapInfo->threatMap->GetSuperiorityAtPos( average );
		//ai->utility->ChatMsg("RL_STATE: MexSpotInf: %f (%f,%f)", (float)MexSpotInf, average.x, average.z );
		
		MexSpotImaginaryInf = ai->knowledge->mapInfo->threatMap->GetImaginarySuperiorityAtPos( average, group, false );

		CurrentSpotInf = ai->knowledge->mapInfo->threatMap->GetSuperiorityAtPos( group->GetPos() );
		//ai->utility->ChatMsg("RL_STATE: Imaginary inf at mexspot (%f,%f) %f", average.x, average.z, (float)MexSpotImaginaryInf);
		
		
		Path* p = NULL;
		if ( MexSpotImaginaryInf == INFERIOR || MexSpotImaginaryInf == EQUAL )
		{
			SuperiorPathLength = 0;
		}
		else
		{
			//ai->utility->ChatMsg("RL_STATE: Finding superior path from (%f,%f) to (%f,%f)", group->GetPos().x, group->GetPos().z, average.x, average.z );

			p = ai->knowledge->mapInfo->pathfindingMap->FindSuperiorPathTo( group, group->GetPos(), average, true );
			//if ( p != NULL )
			//	ai->utility->ChatMsg("Path length: %f", p->GetLength() );
			//else ai->utility->ChatMsg( "RL_STATE: We are inferior at source");
			//ai->utility->ChatMsg("RL_STATE: Done finding superior path");
			SuperiorPathLength = (p == NULL || p->GetLength() == 0 ) ? 0 : 1;
		}

		//Calculate the ID!
		ID = 0;
		unsigned int offset = 1;
		for ( unsigned int m = 0 ; m < stateVars.size() ; m++ )
		{
			unsigned int current = 0;
			switch( m )
			{
			case 0: current = ( realClusters[j].size() < 2 ) ? 0 : ( realClusters[j].size() < 3 ) ? 1 : 2 ; break;
			case 1: current = ( DistMexSpot < 700 ) ? 0 : ( DistMexSpot < 1400 ) ? 1 : ( DistMexSpot < 2500 ) ? 2 : 3; break;
			//case 2: current = MexSpotInf; break;
			case 2: current = MexSpotImaginaryInf; break;
			case 3: current = AirGroup; break;
				//Slow = kbots, medium = tanks and slow aircrafts, fast = fighters, bombers
			case 4: current = ( GroupSpeed < 70 ) ? 0 : ( GroupSpeed < 190 ) ? 1 : 2; break;
			case 5: current = SuperiorPathLength; break;
			case 6: current = CurrentSpotInf; break;
			//default: ai->utility->ChatMsg("RL_State: Unexpected Statevar!");
			}
			ID += current*offset;
			offset *= stateVars[m].numStates;
			//ai->utility->ChatMsg("StateVar: %d. ID: %d. Current: %d", m, ID, current );
		}
		//ai->utility->ChatMsg("RL_State: ID %d", ID );
		terminal = true;
		RL_Action* a = new RL_Action( ai, 0, group, unitIDs, p );
		double val = valueFunction->GetValue( ID, *a );
		possibleStates.insert( make_pair( ID, make_pair( a, val ) ) );
	}


	//ai->utility->ChatMsg("RL_STATE: Possible States: %d", possibleStates.size() );
	int fewestVisits = 100000;
	int fewestVisitsIndex = -1;
	if ( possibleStates.size() > 0 )
	{
		for ( map<int, pair<RL_Action*, double> >::iterator it = possibleStates.begin() ; it != possibleStates.end() ; it++ )
		{
			int v = GetVisitsTo(it->first );
			if ( v < fewestVisits )
			{
				fewestVisits = v;
				fewestVisitsIndex = it->first;
			}
		}
		ID = fewestVisitsIndex;	
		Actions.push_back( possibleStates.find(ID)->second.first );
		if ( fewestVisits == 0 )
			ai->utility->ChatMsg("RL_STATE ID: %d", ID );
	}
	else
		ID = -1;

	map<int, pair<RL_Action*, double> >::iterator it = possibleStates.begin() ;
	while(  it != possibleStates.end() )
	{
		if ( it->first != ID )
		{
			int a = it->first;
			delete it->second.first;
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
	if ( Reader != NULL )
		delete Reader;
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
	for (int i = 0; i < Actions.size(); i++)
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

int RL_State::GetVisitsTo(int stateID)
{
	DataDirs *dirs = ai->callback->GetDataDirs();
	const char* dir = dirs->GetWriteableDir();

	char *path = new char[200];
	strcpy(path, dir);
	strcat(path, "visits.dat");

	fstream *file = new fstream();
	file->open(path, ios::in | ios::binary );

	//file did not exist!!
	if ( !file->is_open() )
	{
		ai->utility->ChatMsg("FILE NOT OPEN");
		file->close();
		return 0;
	}
	file->close();
	file->open( path, ios::out | ios::binary | ios::in );
	int v = ai->utility->ReadFromStateVisitFile( file, stateID );
	file->close();
	return v;
}
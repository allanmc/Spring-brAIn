#include "Scenario.h"
#include "Map.h"
#include "GroupController.h"
#include "MilitaryUnitGroup.h"
#include "Knowledge.h"
using namespace std;
using namespace brainSpace;
using namespace springai;

Scenario::Scenario( AIClasses* aiClasses )
{
	ai = aiClasses;
	int numGroups;
	if ( ai->callback->GetTeamId() == 0 )
		numGroups = 1;
	else numGroups = ( rand()%SCENARIO_MAX_GROUPS ) +1;

	//ai->utility->ChatMsg("Spawning %d groups of military units", numGroups );
	vector<int> ground;
	vector<int> air;
	ground.push_back(120);	//0 - armpw
	ground.push_back(127);	//1 - armrock
	ground.push_back(82);	//2 - armham
	ground.push_back(89);	//3 - armjeth
	ground.push_back(173);	//4 - armwar
	ground.push_back(61);	//5 - armfav
	ground.push_back(70);	//6 - armflash
	ground.push_back(116);	//7 - armpincer
	ground.push_back(151);	//8 - armstump
	ground.push_back(368);	//9 - tawf013
	ground.push_back(88);	//10 - armjanus
	ground.push_back(130);	//11 - armsam
	ground.push_back(-1);	//12 - nullground

	//air.push_back("armpeep");
	air.push_back(-2);	//0 - nullair
	air.push_back(68);	//1 - armfig
	air.push_back(157);	//2 - armthund
	air.push_back(90);	//3 - armkam
	units_all.push_back(ground);
	units_all.push_back(air);

	//spot 1, 6, 7
	Map* m = ai->callback->GetMap();
	vector<SAIFloat3> metalSpots = m->GetResourceMapSpotsPositions(*ai->utility->GetResource("Metal"), NULL);
	if ( metalSpots.size() > 1000 )
		IsMetalMap = true;
	else IsMetalMap = false;
	if ( !IsMetalMap )
	{
		//Find clusters of mex points near each other ( not necessary on metal maps )
		for ( unsigned int i = 0 ; i < metalSpots.size() ; i++ )
		{
			//ai->utility->ChatMsg("Spot %d (%.2f,%.2f,%.2f)", i, metalSpots[i].x, metalSpots[i].y, metalSpots[i].z );
			int count = 0;
			metalSpotClusters.push_back( vector<SAIFloat3>() );
			metalSpotClusters.back().push_back( metalSpots[i] );
			for ( unsigned int j = 0 ; j < metalSpots.size() ; j++ )
			{
				if ( i == j )
					continue;
				double dist = ai->utility->EuclideanDistance( metalSpots[i], metalSpots[j] );
				//ai->utility->ChatMsg("To spot %d (%.2f,%.2f,%.2f): %.2f", j, metalSpots[j].x, metalSpots[j].y, metalSpots[j].z, dist );

				if ( dist < RESOURCE_BUILDING_GROUP_TOLERANCE )
				{
					count++;
					//ai->utility->ChatMsg("i: %d. j: %d", i, j );
					metalSpotClusters.back().push_back( metalSpots[j] );
				}
			}
		}
	}


	SAIFloat3 groupPos;
	groupPos = m->GetStartPos();
	groupPos.y += 50;

	vector<SAIFloat3> ourGroups;

	if ( ai->callback->GetTeamId() == 1 )
	{
		int clusters = rand()%(SCENARIO_MEX_CLUSTERS+1);
		if ( clusters < 1 )
			clusters = 1;

		//ai->utility->ChatMsg("SCENARIO: Starting %d clusters", clusters );
		for ( int i = 0 ; i < clusters ; i++ )
		{
			int size = (rand()%SCENARIO_MEX_CLUSTER_SIZE_MAX+1);
			if ( size < SCENARIO_MEX_CLUSTER_SIZE_MIN )
				size = SCENARIO_MEX_CLUSTER_SIZE_MIN;
			//ai->utility->ChatMsg("SCENARIO: Init cluster, size: %d", size );
			MakeMexCluster( size );
		}
	}

	for ( int i = 0 ; i < numGroups ; i++ )
	{
		bool air = rand()%2;
		bool ok = false;

		groupPos.x = rand()%(m->GetWidth()*8);
		groupPos.z = rand()%(m->GetHeight()*8);
		while (!ok )
		{
			ok = true;

			//Too close to another group check
			for ( unsigned int j = 0 ; j < ourGroups.size() ; j++ )
			{
				if ( ( fabs( (ourGroups[j].x - groupPos.x ) ) < 500.0f ) && (fabs( ourGroups[j].z - groupPos.z ) < 500.0f ) )
				{
					ok = false;
					groupPos.x = rand()%(m->GetWidth()*8);
					groupPos.z = rand()%(m->GetHeight()*8);
					break;
				}
			}
			//Too close to border check
			if ( groupPos.x < 350 )
				groupPos.x += 350;
			else if ( groupPos.x > ( m->GetWidth()*8 )-350 )
				groupPos.x -= 350;
			if ( groupPos.z < 350 )
				groupPos.z += 350;
			else if ( groupPos.z > ( m->GetHeight()*8 )-350 )
				groupPos.z -= 350;
		}


		ourGroups.push_back(groupPos);
		//ai->utility->ChatMsg("SCENARIO: GroupPos (%f,%f,%f)", groupPos.x, groupPos.y, groupPos.z );

		for ( unsigned int j = 0 ; j < 3 ; j++ )
		{
			int type = units_all[air][rand()%units_all[air].size()];
			if ( type > -1 )
			{
				for ( unsigned int k = 0 ; k < 4 ; k++ )
				{
					SAIFloat3 unitPos = groupPos;
					unitPos.x += 30*k;
					unitPos.z += 30*j;
					//ai->utility->ChatMsg("Planting unit: (%f,%f,%f)", unitPos.x, unitPos.y, unitPos.z );
					Unit* u = ai->utility->GiveUnit( type, unitPos );
					delete u;
					u = NULL;
				}
			}
		}
	}

	ai->knowledge->mapInfo->threatMap->Update();
	delete m;
	m = NULL;
}

Scenario::~Scenario()
{
}

void Scenario::MakeMexCluster( unsigned int size )
{
	Map* m = ai->callback->GetMap();
	SAIFloat3 thisClustersPos;
	vector<SAIFloat3> metalSpots = m->GetResourceMapSpotsPositions(*ai->utility->GetResource("Metal"), NULL);
	int randIndex = rand()%metalSpots.size();
	thisClustersPos = metalSpots[randIndex];

	if (IsMetalMap )
	{
		bool ok = false;
		vector<SAIFloat3> currentCluster;

		//Find a suitable startposition
		while (!ok)
		{
			ok = true;
			if ( !m->IsPossibleToBuildAt( *ai->utility->GetUnitDef("armmex"), metalSpots[randIndex], 0 ) )
				ok = false;

			//Check for existing clusters
			for ( unsigned int j = 0 ; j < mexClusterPositions.size() ; j++ )
			{
				if ( ai->utility->EuclideanDistance( mexClusterPositions[j], thisClustersPos ) < CLUSTER_DISTANCE )
				{
					//Interference with one already existing cluster
					ok = false;
					break;
				}
			}
			if ( !ok )
			{
				randIndex = rand()%metalSpots.size();
				thisClustersPos = metalSpots[randIndex];
				continue;
			}
		}
		Unit* u = ai->utility->GiveUnit("armmex", thisClustersPos );
		delete u;
		u = NULL;
		//ai->utility->ChatMsg("SCENARIO: Mex spawned at (%f,%f,%f)", thisClustersPos.x, thisClustersPos.y, thisClustersPos.z );
		currentCluster.push_back( thisClustersPos );
		for ( unsigned int k = 1 ; k < size ; k++ )
		{		
			unsigned int iterations = 0;
			for ( unsigned int i = rand()%metalSpots.size() ; iterations < metalSpots.size() ; (i == metalSpots.size()-1 ) ? i = 0 : i++ )
			{
				iterations++;
				bool thisMetalSpotIsGood = true;
				//See if this metalspot obeys the distance rules given the rest of this cluster
				for ( unsigned int j = 0 ; j < currentCluster.size() ; j++ )
				{
					//Spot already in cluster (should never happen)
					if ( currentCluster[j].x == metalSpots[i].x && currentCluster[j].z == metalSpots[i].x )
						thisMetalSpotIsGood = false;
					if ( !m->IsPossibleToBuildAt( *ai->utility->GetUnitDef("armmex"), metalSpots[i], 0 ) )
						thisMetalSpotIsGood = false;

					//Check for minimum distance between units in cluster
					if ( ai->utility->EuclideanDistance( currentCluster[j], metalSpots[i] ) >  RESOURCE_BUILDING_GROUP_TOLERANCE || 
						ai->utility->EuclideanDistance( currentCluster[j], metalSpots[i] ) <  100
						)
					{
						thisMetalSpotIsGood = false;
					}
					if ( metalSpots[i].x < 200 || metalSpots[i].x > ((m->GetWidth()*8)-200) || metalSpots[i].z < 200 || metalSpots[i].z > ((m->GetHeight()*8)-200) )
					{
						thisMetalSpotIsGood = false;
					}
				}

				if ( thisMetalSpotIsGood )
				{
					Unit* u = ai->utility->GiveUnit("armmex", metalSpots[i] );
					delete u;
					u = NULL;
					currentCluster.push_back( metalSpots[i]);
					//ai->utility->ChatMsg("SCENARIO: Mex spawned at (%f,%f,%f). New cluster size %d", metalSpots[i].x, metalSpots[i].y, metalSpots[i].z, currentCluster.size() );
					break;
				}
				else if ( iterations == metalSpots.size() - 1 )
					ai->utility->ChatMsg("SCENARIO: No suitable cluster could be made");
			}
		}
	}
	else
	{
		for ( unsigned int i = 0 ; i < metalSpotClusters.size() ; i++ )
		{
			bool ok = true;
			//Find one of appropriate size
			if ( metalSpotClusters[i].size() < size )
				continue;

			//Check for various things that would make the cluster inappropriate
			for ( unsigned int j = 1 ; j < metalSpotClusters[i].size() ; j++ )
			{
				//Height difference between spots
				if ( ( metalSpotClusters[i].front().y > metalSpotClusters[i][j].y + 500.0f ) ||
					( metalSpotClusters[i].front().y +500.0f < metalSpotClusters[i][j].y ) )
				{
					ok = false;
					break;
				}

				//Already something there...
				if ( !m->IsPossibleToBuildAt( *ai->utility->GetUnitDef("armmex"), metalSpotClusters[i][j], 0 ) )
				{
					ok = false;
					break;
				}
			}
			if ( !ok )
			{
				continue;
			}

			//Check for existing clusters
			for ( unsigned int j = 0 ; j < mexClusterPositions.size() ; j++ )
			{
				if ( ai->utility->EuclideanDistance( mexClusterPositions[j], thisClustersPos ) < CLUSTER_DISTANCE )
				{
					//Interference with one already existing cluster
					ok = false;
					break;
				}
			}

			if ( ok )
			{
				unsigned int count = 0;
				//ai->utility->ChatMsg("SCENARIO: Spawning %d of %d mexes", size, metalSpotClusters[i].size() );
				for ( unsigned int j = 0 ; j < metalSpotClusters[i].size() ; j++ )
				{
					ai->utility->ChatMsg("MexSpot: (%f,%f,%f)", metalSpotClusters[i][j].x, metalSpotClusters[i][j].z );
					Unit* u = ai->utility->GiveUnit("armmex", metalSpotClusters[i][j] );
					if ( u != NULL )
						count++;
					else ai->utility->ChatMsg("Spawned mex which was null");
					//Are we done now?
					if ( count == size )
						break;
				}
				//ai->utility->ChatMsg("SCENARIO: Spawned %d mexes", count );
				//Register the position of the cluster in the class variable
				mexClusterPositions.push_back( metalSpotClusters[i][0] );

				break;
			}
		}
	}
	delete m;
	m = NULL;
}
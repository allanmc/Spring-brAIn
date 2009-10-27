#include "PathfindingMap.h"
#include "Knowledge.h"

using namespace std;
using namespace springai;
using namespace brainSpace;

PathfindingMap::PathfindingMap( AIClasses* aiClasses ) : BrainMap( aiClasses, 4 )
{

	//Slopemap has Resolution = 2, where PFmap has Resolution = 4.
	vector<float> slopeMap = ai->callback->GetMap()->GetSlopeMap();
	SlopeMap = slopeMap;
	for ( int z = 0 ; z < MapHeight ; z++ )
		for ( int x = 0 ; x < MapWidth ; x++ )
			ResetSlope( x, z );
}


PathfindingMap::~PathfindingMap()
{
}


void PathfindingMap::EffectCell(int index, float value)
{
}


void PathfindingMap::Update()
{
	ai->utility->RemoveGraphics(FigureID);
	FigureID = 0;

	for ( int j = 0 ; j < MapHeight ; j++ )
		for ( int i = 0 ; i < MapWidth ; i++ )
		{
			if ( MapArray[ j*MapWidth + i ] > 0.40f )
			{
				//ai->utility->Log( ALL, SLOPEMAP, "(%d, %d) is equal to 10", i, j );
				SAIFloat3 start, end;
				start.x = i*Resolution;
				start.y = 40;
				start.z = j*Resolution + 0.5*Resolution;

				end.x = (i+1)*Resolution;
				end.y = 40;
				end.z = j*Resolution + 0.5*Resolution;


				int id = ai->utility->DrawLine( start, end, false, Resolution*2, FigureID );
				if(FigureID == 0) FigureID = id;
			}
		}
}

void PathfindingMap::AddBuilding(Unit* unit)
{
	SAIFloat3 pos = unit->GetPos();
	int xSize = unit->GetDef()->GetXSize();
	int zSize = unit->GetDef()->GetZSize();
	int topCell = (pos.z-zSize)/Resolution;
	int bottomCell = (pos.z+zSize)/Resolution;
	int leftCell = (pos.x-xSize)/Resolution;
	int rightCell = (pos.x+xSize)/Resolution;


	for ( int i = topCell ; i <= bottomCell ; i++ )
		for ( int j = leftCell ; j <= rightCell ; j++ )
		{
			if ( i > MapHeight || i < 0 || j > MapWidth || j < 0 )
				continue;

			MapArray[i*MapWidth + j] = 10.0f; //Now this tile is impassable.
		}
}


void PathfindingMap::AddHypotheticalBuilding(UnitDef* unit, SAIFloat3 pos)
{
	int xSize = unit->GetXSize();
	int zSize = unit->GetZSize();
	int topCell = (pos.z-zSize)/Resolution;
	int bottomCell = (pos.z+zSize)/Resolution;
	int leftCell = (pos.x-xSize)/Resolution;
	int rightCell = (pos.x+xSize)/Resolution;


	for ( int i = topCell ; i <= bottomCell ; i++ )
		for ( int j = leftCell ; j <= rightCell ; j++ )
		{
			if ( i > MapHeight || i < 0 || j > MapWidth || j < 0 )
				continue;

			MapArray[i*MapWidth + j] = 10.0f; //Now this tile is impassable.
		}
}


void PathfindingMap::RemoveHypotheticalBuilding(UnitDef* unit, SAIFloat3 pos)
{
	int xSize = unit->GetXSize();
	int zSize = unit->GetZSize();
	int topCell = (pos.z-zSize)/Resolution;
	int bottomCell = (pos.z+zSize)/Resolution;
	int leftCell = (pos.x-xSize)/Resolution;
	int rightCell = (pos.x+xSize)/Resolution;


	for ( int i = topCell ; i <= bottomCell ; i++ )
		for ( int j = leftCell ; j <= rightCell ; j++ )
		{
			if ( i > MapHeight || i < 0 || j > MapWidth || j < 0 )
				continue;
			ResetSlope( j, i );
		}
}


void PathfindingMap::RemoveBuilding(Unit* unit)
{
	SAIFloat3 pos = unit->GetPos();
	int xSize = unit->GetDef()->GetXSize();
	int zSize = unit->GetDef()->GetZSize();
	int topCell = (pos.z-zSize)/Resolution;
	int bottomCell = (pos.z+zSize)/Resolution;
	int leftCell = (pos.x-xSize)/Resolution;
	int rightCell = (pos.x+xSize)/Resolution;

	for ( int i = topCell ; i <= bottomCell ; i++ )
		for ( int j = leftCell ; j <= rightCell ; j++ )
		{
			if ( i > MapHeight || i < 0 || j > MapWidth || j < 0 )
				continue;
			ResetSlope( j, i );
		}
}

void PathfindingMap::ResetSlope( int xTile, int zTile )
{

	int width = ai->callback->GetMap()->GetWidth()/2;
	float slope1 = SlopeMap[xTile*2+width*2*zTile];
	float slope2 = SlopeMap[((xTile*2)+1)+width*2*zTile];
	float slope3 = SlopeMap[(xTile*2)+width*2*zTile+width];
	float slope4 = SlopeMap[((xTile*2)+1)+width*2*zTile+width];
	float maxSlope =  max( slope4, max( slope3, max( slope1, slope2 ) ) );
	MapArray[zTile*MapWidth + xTile] = maxSlope;
}

vector<PathfindingNode*> PathfindingMap::FindPathTo( UnitDef* pathfinder, SAIFloat3 start, SAIFloat3 destination )
{
	int goalXIndex = destination.x/Resolution;
	int goalZIndex = destination.z/Resolution;

	if ( MapArray[ goalZIndex*MapWidth + goalXIndex] > pathfinder->GetMoveData()->GetMaxSlope() )
	{
		ai->utility->ChatMsg( "I cannot reach the destination!!" );
	}
	map<int, PathfindingNode*> closedSet;
	map<int, PathfindingNode*> openSet;
	start.x += 0.5*Resolution;
	start.z += 0.5*Resolution;
	PathfindingNode* startNode = new PathfindingNode( start, start.x/Resolution, start.z/Resolution, 0, ai->utility->EuclideanDistance( start, destination ), ai->utility->EuclideanDistance( start, destination ), MapArray[ (int)start.z/Resolution*MapWidth + (int)start.x/Resolution]  );
	
	openSet.insert( make_pair((int)start.z/Resolution*MapWidth + (int)start.x/Resolution, startNode ) );
	
	//ai->utility->Log( ALL, SLOPEMAP, "Starting at tile(%d, %d)", startNode->XIndex, startNode->ZIndex );
	while (!openSet.empty() )
	{
		//ai->utility->Log( ALL, SLOPEMAP, "Openset is not empty" );
		PathfindingNode* current;
		int currentIndex = -1;
		float lowestF = 90000000.0f;

		/** FIND IN OPENSET **/
		for ( map<int, PathfindingNode*>::iterator i = openSet.begin() ; i != openSet.end() ; i++ )
		{
			if ( i->second->Fscore < lowestF )
			{
				lowestF = i->second->Fscore;
				current = i->second;
				currentIndex = i->first;
			}
		}



		/** CHECK FOR GOAL **/
		//ai->utility->Log( ALL, SLOPEMAP, "\nCurr(%d, %d)G: %f. H = %f. F = %f", current->XIndex, current->ZIndex, current->Gscore, current->Hscore, current->Fscore );
		if ( current->XIndex == goalXIndex && current->ZIndex == goalZIndex )
		{
			//ai->utility->Log( ALL, SLOPEMAP, "We have found the goal node" );
			vector<PathfindingNode*> shortestPath = ReconstructPath( current );

			DeleteUnusedPathfindingNodes( closedSet, openSet, shortestPath );

			return shortestPath;
		}

		openSet.erase( currentIndex );
		closedSet.insert( make_pair( current->ZIndex*MapWidth + current->XIndex, current ) );

		for ( int z = current->ZIndex-1 ; z <= current->ZIndex+1 ; z++ ) //for each neighbour of current node...
		{
			for ( int x = current->XIndex-1 ; x <= current->XIndex+1 ; x++ )
			{
				if ( z < 0 || z > MapHeight || x < 0 || x > MapHeight ) //out of bounds check
					continue;
				if ( z == current->ZIndex && x == current->XIndex ) //skip the current node
				{
					//ai->utility->Log( ALL, SLOPEMAP, "Skipping the current node" );
					continue;
				}


				map<int, PathfindingNode*>::iterator it = closedSet.find( z*MapWidth + x );
				if ( it != closedSet.end() )
				{
					//ai->utility->Log( ALL, SLOPEMAP, "(%d, %d) was in closedset", it->second->XIndex, it->second->ZIndex );
					continue;
				}
				bool neighbourInOpenSet = true;
				it = openSet.find( z*MapWidth + x );
				if ( it == openSet.end() )
					neighbourInOpenSet = false;

				SAIFloat3 pos;
				pos.x = x*Resolution + 0.5*Resolution;
				pos.z = z*Resolution + 0.5*Resolution;
				pos.y = 50;

				float tentativeGScore = current->Gscore + ai->utility->EuclideanDistance( current->Pos, pos );
				//ai->utility->Log( ALL, SLOPEMAP, "Tentative g score %f for neighbour (%d, %d)", tentativeGScore, x, z );
				//ai->utility->Log( ALL, SLOPEMAP, "Neighbour pos: (%f, %f) tile:(%d, %d)", neighbour.Pos.x, neighbour.Pos.z, neighbour.XIndex, neighbour.ZIndex );
				bool tentativeIsBetter = true;
				


				PathfindingNode* neighbour;
				if ( !neighbourInOpenSet )
				{
					neighbour = new PathfindingNode();
					neighbour->Pos.x = x*Resolution + 0.5*Resolution;
					neighbour->Pos.z = z*Resolution + 0.5*Resolution;
					neighbour->Pos.y = 50;
					neighbour->XIndex = x;
					neighbour->ZIndex = z;
					neighbour->Slope = MapArray[ z*MapWidth + x ];
					
					//ai->utility->Log( ALL, SLOPEMAP, "(%d, %d) not in openset", neighbour->XIndex, neighbour->ZIndex );
					if ( neighbour->Slope > pathfinder->GetMoveData()->GetMaxSlope() )
					{
						closedSet.insert( make_pair( z*MapWidth + x, neighbour ) );
						tentativeIsBetter = false;
					}
					else
						openSet.insert( make_pair( z*MapWidth + x, neighbour ) );
				}
				else
				{
					neighbour = it->second;
					if ( tentativeGScore < neighbour->Gscore )
					{
						//ai->utility->Log( ALL, SLOPEMAP, "Tentative was better: %f. Neighbour: %f", tentativeGScore, neighbour->Gscore );
						tentativeIsBetter = true;
					}
					else
					{
						tentativeIsBetter = false;
						//ai->utility->Log( ALL, SLOPEMAP, "Tentative is worse" );
					}
				}

				if ( tentativeIsBetter )
				{
					//PathfindingNode *n = new PathfindingNode( current.Pos, current.XIndex, current.ZIndex, current.Gscore, current.Hscore, current.Fscore );
					neighbour->CameFrom = current;
					neighbour->Gscore = tentativeGScore;
					neighbour->Hscore = ai->utility->EuclideanDistance( neighbour->Pos, destination );
					neighbour->Fscore = neighbour->Gscore + neighbour->Hscore;
					//ai->utility->Log( ALL, SLOPEMAP, "New connection - Current(%d,%d) to Neighbour(%d, %d) G = %f. H = %f. F= %f", neighbour->CameFrom->XIndex, neighbour->CameFrom->ZIndex, neighbour->XIndex, neighbour->ZIndex, neighbour->Gscore, neighbour->Hscore, neighbour->Fscore );	
				}
			}
		}
	}
	vector<PathfindingNode*> emptyVector;
	return emptyVector;
}


bool PathfindingMap::IsPossibleToEscapeFrom( UnitDef* pathfinder, UnitDef* building, SAIFloat3 buildPosition, SAIFloat3 escapeFrom, SAIFloat3 escapeTo )
{
	AddHypotheticalBuilding( building, buildPosition );
	vector<PathfindingNode*> path = FindPathTo( pathfinder, escapeFrom, escapeTo );
	RemoveHypotheticalBuilding( building, buildPosition );
	return ( path.size() > 0 );
}


vector<PathfindingNode*> PathfindingMap::ReconstructPath( PathfindingNode* currentNode )
{
	vector<PathfindingNode*> nodes;
	nodes.push_back( currentNode );


	while ( currentNode->CameFrom != NULL )
	{
		//ai->utility->Log( ALL, SLOPEMAP, "(%d, %d) has a predecessor (%d, %d)", currentNode->XIndex, currentNode->ZIndex, currentNode->CameFrom->XIndex, currentNode->CameFrom->ZIndex );
		nodes.push_back( currentNode->CameFrom );
		currentNode = currentNode->CameFrom;
	}

	return nodes;
}


void PathfindingMap::DeleteUnusedPathfindingNodes( map<int, PathfindingNode*> closedSet, map<int, PathfindingNode*> openSet, vector<PathfindingNode*> shortestPath )
{
	vector<PathfindingNode*> nodesToDelete;
	map<int, PathfindingNode*>::iterator it;
	bool exists = false;

	for ( map<int, PathfindingNode*>::iterator it = openSet.begin() ; it != openSet.end() ; it++ )
	{
		exists = false;
		for ( int k = 0 ; k < (int)shortestPath.size() ; k++ )
		{
			if ( it->second->XIndex == shortestPath[k]->XIndex && it->second->ZIndex == shortestPath[k]->ZIndex )
			{
				exists = true;
				break;
			}
		}
		if ( !exists )
		{
			nodesToDelete.push_back( it->second );
			continue;
		}
	}
	
	for ( map<int, PathfindingNode*>::iterator it = closedSet.begin() ; it != closedSet.end() ; it++ )
	{
		exists = false;
		for ( int k = 0 ; k < (int)shortestPath.size() ; k++ )
		{
			if ( it->second->XIndex == shortestPath[k]->XIndex && it->second->ZIndex == shortestPath[k]->ZIndex )
			{
				exists = true;
				break;
			}
		}
		if ( !exists )
		{
			nodesToDelete.push_back( it->second );
			continue;
		}
	}

	ai->utility->Log( ALL, SLOPEMAP, "Deleting %d unused nodes", nodesToDelete.size() );
	for ( int k = 0 ; k < (int)nodesToDelete.size() ; k++ )
		delete nodesToDelete[k];
}
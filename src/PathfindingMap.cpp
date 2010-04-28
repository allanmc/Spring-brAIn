#include "PathfindingMap.h"
#include "Knowledge.h"

using namespace std;
using namespace springai;
using namespace brainSpace;

PathfindingMap::PathfindingMap( AIClasses* aiClasses ) : BrainMap( aiClasses, 4 )
{

	//Slopemap has Resolution = 2, where PFmap has Resolution = 4.
	ResetEntireMap();
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
			if ( MapArray[ j*MapWidth + i ] > 5.0f )
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
	UnitDef *def = unit->GetDef();
	int xSize = def->GetXSize()*8;
	int zSize = def->GetZSize()*8;
	int topCell = (pos.z-zSize/2)/Resolution;
	int bottomCell = (pos.z+zSize/2)/Resolution;
	int leftCell = (pos.x-xSize/2)/Resolution;
	int rightCell = (pos.x+xSize/2)/Resolution;
	delete def;


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
	int xSize = unit->GetXSize()*8;
	int zSize = unit->GetZSize()*8;
	int topCell = (pos.z-zSize/2)/Resolution;
	int bottomCell = (pos.z+zSize/2)/Resolution;
	int leftCell = (pos.x-xSize/2)/Resolution;
	int rightCell = (pos.x+xSize/2)/Resolution;


	for ( int i = topCell ; i <= bottomCell ; i++ )
		for ( int j = leftCell ; j <= rightCell ; j++ )
		{
			if ( i > MapHeight || i < 0 || j > MapWidth || j < 0 )
				continue;
			backUp[i*MapWidth + j] = MapArray[i*MapWidth + j];//remember the old value
			MapArray[i*MapWidth + j] = 10.0f; //Now this tile is impassable.
		}
}


void PathfindingMap::RemoveHypotheticalBuilding(UnitDef* unit, SAIFloat3 pos)
{
	int xSize = unit->GetXSize()*8;
	int zSize = unit->GetZSize()*8;
	int topCell = (pos.z-zSize/2)/Resolution;
	int bottomCell = (pos.z+zSize/2)/Resolution;
	int leftCell = (pos.x-xSize/2)/Resolution;
	int rightCell = (pos.x+xSize/2)/Resolution;


	for ( int i = topCell ; i <= bottomCell ; i++ )
		for ( int j = leftCell ; j <= rightCell ; j++ )
		{
			if ( i > MapHeight || i < 0 || j > MapWidth || j < 0 )
				continue;
			MapArray[i*MapWidth + j] = backUp[i*MapWidth + j];
			//ResetSlope( j, i );
		}
	backUp.clear();
}


void PathfindingMap::RemoveBuilding(Unit* unit)
{
	SAIFloat3 pos = unit->GetPos();
	UnitDef *def = ai->knowledge->selfInfo->baseInfo->GetUnitDef(unit->GetUnitId());
	if(def == NULL)
		return;
	int xSize = def->GetXSize()*8;
	int zSize = def->GetZSize()*8;
	int topCell = (pos.z-zSize/2)/Resolution;
	int bottomCell = (pos.z+zSize/2)/Resolution;
	int leftCell = (pos.x-xSize/2)/Resolution;
	int rightCell = (pos.x+xSize/2)/Resolution;

	vector<int> deadBuildingTiles;

	for ( int i = topCell ; i <= bottomCell ; i++ )
		for ( int j = leftCell ; j <= rightCell ; j++ )
		{
			deadBuildingTiles.push_back( i*MapWidth+j );
		}
	


	vector<Unit*> unitsInRange = ai->knowledge->selfInfo->baseInfo->GetUnitsInRange( pos, max( xSize, zSize ) + Resolution );
	for ( unsigned int i = 0 ; i < unitsInRange.size() ; i++ )
	{
		SAIFloat3 unitPos = unitsInRange[i]->GetPos();
		UnitDef *def1 = ai->knowledge->selfInfo->baseInfo->GetUnitDef(unitsInRange[i]->GetUnitId());
		if(def1 == NULL)
			continue;
		int unitXsize = def1->GetXSize()*8;
		int unitZsize = def1->GetZSize()*8;

		int unitTopCell = (unitPos.z-unitZsize/2)/Resolution;
		int unitBottomCell = (unitPos.z+unitZsize/2)/Resolution;
		int unitLeftCell = (unitPos.x-unitXsize/2)/Resolution;
		int unitRightCell = (unitPos.x+unitXsize/2)/Resolution;


		vector<int> indicesToDelete;

		for ( int k = unitTopCell ; k <= unitBottomCell ; k++ )
		{
			for ( int j = unitLeftCell ; j <= unitRightCell ; j++ )
			{
				if ( k > MapHeight || k < 0 || j > MapWidth || j < 0 )
					continue;
				for ( int h = 0 ; h < (int)deadBuildingTiles.size() ; h++ )
				{
					if ( deadBuildingTiles[h] == k*MapWidth+j )
					{
						//if another building also occupies the tile previously 
						//occupied by the now dead building, push it to the vector
						indicesToDelete.push_back( h );
					}
				}
			}
		}

		for ( int k = 0 ; k < (int)indicesToDelete.size() ; k++ )
		{
			//Erase all the selected indices
			//The index value must be decremented as we iterate through this vector
			deadBuildingTiles.erase( deadBuildingTiles.begin()+(indicesToDelete[k]-k) );
		}	
	}

	for ( int k = 0 ; k < (int)deadBuildingTiles.size() ; k++ )
	{
		int xTile = deadBuildingTiles[k]%MapWidth;
		int zTile = deadBuildingTiles[k]/MapWidth;
		ResetSlope( xTile, zTile );
	}
}

void PathfindingMap::ResetSlope( int xTile, int zTile )
{
	Map *map = ai->callback->GetMap();
	int width = map->GetWidth()/2;
	delete map;
	float slope1 = SlopeMap[xTile*2+width*2*zTile];
	float slope2 = SlopeMap[((xTile*2)+1)+width*2*zTile];
	float slope3 = SlopeMap[(xTile*2)+width*2*zTile+width];
	float slope4 = SlopeMap[((xTile*2)+1)+width*2*zTile+width];
	float maxSlope =  max( slope4, max( slope3, max( slope1, slope2 ) ) );
	MapArray[zTile*MapWidth + xTile] = maxSlope;
}

Path* PathfindingMap::FindPathTo( UnitDef* pathfinder, SAIFloat3 start, SAIFloat3 destination, bool debug )
{
	if (debug) ai->utility->Log(ALL, PATHFIND,  "FindPathTo start, resolution: %d",Resolution );
	int goalXIndex = destination.x/Resolution;
	int goalZIndex = destination.z/Resolution;
	MoveData *move = pathfinder->GetMoveData();
	
	float maxSlope;
	
	if ( move != NULL )
	{
		maxSlope = move->GetMaxSlope();
		delete move;
	}
	else maxSlope = -1;


	if ( MapArray[ goalZIndex*MapWidth + goalXIndex] > maxSlope)
	{
		ai->utility->Log(ALL, PATHFIND,  "Svend: I cannot reach the destination!!" );
		vector<SAIFloat3> emptyResult;
		Path* p = new Path(ai, emptyResult );
		return p;
	}
	if (debug) ai->utility->Log(ALL, PATHFIND,  "FindPathTo destination is good" );
	map<int, PathfindingNode*> closedSet;
	map<int, PathfindingNode*> openSet;
	//start.x += 0.5*Resolution;
	//start.z += 0.5*Resolution;
	PathfindingNode* startNode = new PathfindingNode( start, start.x/Resolution, start.z/Resolution, 0, ai->utility->EuclideanDistance( start, destination ), ai->utility->EuclideanDistance( start, destination ), MapArray[ (int)start.z/Resolution*MapWidth + (int)start.x/Resolution]  );
	
	openSet.insert( make_pair((int)start.z/Resolution*MapWidth + (int)start.x/Resolution, startNode ) );
	if (debug) ai->utility->Log(ALL, PATHFIND,  "FindPathTo inserted first node in openset" );
	if (debug) ai->utility->Log( ALL, PATHFIND, "Starting at tile(%d, %d)", startNode->XIndex, startNode->ZIndex );
	while (!openSet.empty() )
	{
		if (debug) ai->utility->Log( ALL, PATHFIND, "Openset is not empty" );
		PathfindingNode* current = NULL;
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

		if (current == NULL)
		{
			if (debug) ai->utility->Log(ALL, PATHFIND,  "FindPathTo Current node is null" );
			vector<SAIFloat3> emptyResult;
			DeleteNodes( closedSet, openSet );
			Path* p = new Path( ai, emptyResult );
			return p;
		}
		if (debug) ai->utility->Log(ALL, PATHFIND,  "FindPathTo Current node is NOT null" );

		/** CHECK FOR GOAL **/
		if (debug) ai->utility->Log( ALL, SLOPEMAP, "\nCurr(%d, %d)G: %f. H = %f. F = %f", current->XIndex, current->ZIndex, current->Gscore, current->Hscore, current->Fscore );
		if ( current->XIndex == goalXIndex && current->ZIndex == goalZIndex )
		{
			if (debug) ai->utility->Log( ALL, PATHFIND, "We have found the goal node" );
			vector<SAIFloat3> shortestPath = ReconstructPath( current );

			DeleteNodes( closedSet, openSet );
			Path* p = new Path(ai, shortestPath );
			return p;
		}

		openSet.erase( currentIndex );
		closedSet.insert( make_pair( current->ZIndex*MapWidth + current->XIndex, current ) );
		if (debug) ai->utility->Log( ALL, PATHFIND, "Current moved to closed set" );

		for ( int z = current->ZIndex-1 ; z <= current->ZIndex+1 ; z++ ) //for each neighbour of current node...
		{
			for ( int x = current->XIndex-1 ; x <= current->XIndex+1 ; x++ )
			{
				if ( z < 0 || z > MapHeight-1 || x < 0 || x > MapWidth-1 ) 
				{
					//out of bounds check
					if (debug) ai->utility->Log( ALL, PATHFIND, "Neighbour out of bounds" );
					continue;
				}
				if ( z == current->ZIndex && x == current->XIndex ) //skip the current node
				{
					if (debug) ai->utility->Log( ALL, PATHFIND, "Skipping the current node, we are twins" );
					continue;
				}


				map<int, PathfindingNode*>::iterator it = closedSet.find( z*MapWidth + x );
				if ( it != closedSet.end() )
				{
					if (debug) ai->utility->Log( ALL, PATHFIND, "(%d, %d) was in closedset", it->second->XIndex, it->second->ZIndex );
					continue;
				}
				bool neighbourInOpenSet = true;
				it = openSet.find( z*MapWidth + x );
				if ( it == openSet.end() )
				{
					neighbourInOpenSet = false;
					if (debug) ai->utility->Log( ALL, PATHFIND, "(%d, %d) was NOT in openset", x, z );
				}

				SAIFloat3 pos;
				pos.x = x*Resolution + 0.5*Resolution;
				pos.z = z*Resolution + 0.5*Resolution;
				pos.y = 50;

				float tentativeGScore = current->Gscore + ai->utility->EuclideanDistance( current->Pos, pos );
				if (debug) ai->utility->Log( ALL, PATHFIND, "Tentative g score %f for neighbour (%d, %d)", tentativeGScore, x, z );
				bool tentativeIsBetter = true;
				


				PathfindingNode* neighbour;
				if ( !neighbourInOpenSet )
				{
					neighbour = new PathfindingNode();
					neighbour->Pos = pos;
					//ai->utility->Log( ALL, PATHFIND, "neighbour pos: %f,%f", pos.x,pos.z );
					neighbour->XIndex = x;
					//ai->utility->Log( ALL, PATHFIND, "neighbour x: %d", x);
					neighbour->ZIndex = z;
					//ai->utility->Log( ALL, PATHFIND, "neighbour z: %d", z);
					if(debug)  ai->utility->Log( ALL, PATHFIND, "neighbour x: %d, z: %d", x, z );
					if ( ( z*MapWidth + x ) >= MapWidth*MapHeight )
						ai->utility->Log( ALL, PATHFIND, "Fuck dig!! %d. MapWidth: %d. MapHeight: %d", z*MapWidth+x, MapWidth, MapHeight );
					
					//ai->utility->Log( ALL, PATHFIND, "mapwidth: %d, mapheight: %d", MapWidth, MapHeight);
					neighbour->Slope = MapArray[ z*MapWidth + x ];
					if(debug) ai->utility->Log( ALL, PATHFIND, "New neighbour initialised, slope: %f ", neighbour->Slope );
					bool suckyNeighbour = false;
					if(current->ZIndex > neighbour->ZIndex || current->ZIndex < neighbour->ZIndex)
					{
						if(current->XIndex > neighbour->XIndex)
						{
							int tmpX = x + 1;
							if( tmpX >= 0 && tmpX < MapWidth && MapArray[ z*MapWidth + tmpX ] > maxSlope)
							{
								suckyNeighbour = true;
							}
						}
						else if(current->XIndex < neighbour->XIndex)
						{
							int tmpX = x - 1;
							if( tmpX >= 0 && tmpX < MapWidth && MapArray[ z*MapWidth + tmpX ] > maxSlope)
							{
								suckyNeighbour = true;
							}
						}
					}
					if (debug) ai->utility->Log( ALL, PATHFIND, "X check done.. %d", suckyNeighbour );
					if(current->XIndex > neighbour->XIndex || current->XIndex < neighbour->XIndex)
					{
						if(current->ZIndex > neighbour->ZIndex)
						{
							int tmpZ = z + 1;
							if( tmpZ >= 0 && tmpZ < MapHeight && MapArray[ tmpZ*MapWidth + x ] > maxSlope)
							{
								suckyNeighbour = true;
							}
						}
						else if(current->ZIndex < neighbour->ZIndex)
						{
							int tmpZ = z - 1;
							if( tmpZ >= 0 && tmpZ < MapHeight && MapArray[ tmpZ*MapWidth + x ] > maxSlope)
							{
								suckyNeighbour = true;
							}
						}
					}
					if (debug) ai->utility->Log( ALL, PATHFIND, "Z check done.. %d", suckyNeighbour );			

					
					if ( suckyNeighbour )
					{
						delete neighbour;
						continue;
					}

					if ( neighbour->Slope > maxSlope)
					{
						if (debug) ai->utility->Log( ALL, PATHFIND, "Bad neighbour..");
						closedSet.insert( make_pair( z*MapWidth + x, neighbour ) );
						tentativeIsBetter = false;
					}
					else
					{
						if (debug) ai->utility->Log( ALL, PATHFIND, "Good neighbour..");
						openSet.insert( make_pair( z*MapWidth + x, neighbour ) );
					}
				}
				else
				{
					if (debug) ai->utility->Log( ALL, PATHFIND, "Already in openset");
					neighbour = it->second;
					if ( tentativeGScore < neighbour->Gscore )
					{
						if (debug) ai->utility->Log( ALL, PATHFIND, "Tentative was better: %f. Neighbour: %f", tentativeGScore, neighbour->Gscore );
						tentativeIsBetter = true;
					}
					else
					{
						tentativeIsBetter = false;
						if (debug) ai->utility->Log( ALL, PATHFIND, "Tentative is worse" );
					}
				}

				if ( tentativeIsBetter )
				{
					//PathfindingNode *n = new PathfindingNode( current.Pos, current.XIndex, current.ZIndex, current.Gscore, current.Hscore, current.Fscore );
					neighbour->CameFrom = current;
					neighbour->Gscore = tentativeGScore;
					neighbour->Hscore = ai->utility->EuclideanDistance( neighbour->Pos, destination );
					neighbour->Fscore = neighbour->Gscore + neighbour->Hscore;
					if (debug) ai->utility->Log( ALL, PATHFIND, "New connection - Current(%d,%d) to Neighbour(%d, %d) G = %f. H = %f. F= %f", neighbour->CameFrom->XIndex, neighbour->CameFrom->ZIndex, neighbour->XIndex, neighbour->ZIndex, neighbour->Gscore, neighbour->Hscore, neighbour->Fscore );	
				}
			}
		}
	}
	if (debug) ai->utility->Log(ALL, PATHFIND, "Openset is empty without finding result");
	vector<SAIFloat3> emptyResult;
	DeleteNodes(closedSet, openSet);
	Path* p = new Path(ai, emptyResult );
	return p;
}

bool PathfindingMap::PathExists( UnitDef* pathfinder, SAIFloat3 escapeFrom, SAIFloat3 escapeTo, bool debug )
{
	ai->utility->Log(ALL, MISC, "Trying to escape...");
	Path* p = FindPathTo( pathfinder, escapeFrom, escapeTo, debug );
	bool b = (p->GetPoints().size() > 0);
	delete p;
	return b;
}

bool PathfindingMap::IsPossibleToEscapeFrom( UnitDef* pathfinder, springai::UnitDef* building, SAIFloat3 buildPosition, SAIFloat3 escapeFrom, SAIFloat3 escapeTo )
{
	
	ai->utility->Log(ALL, MISC, "Trying to escape...");
	AddHypotheticalBuilding( building, buildPosition );
	bool retVal = PathExists(pathfinder, escapeFrom, escapeTo);
	RemoveHypotheticalBuilding( building, buildPosition );
	return retVal;
}


vector<SAIFloat3>  PathfindingMap::ReconstructPath( PathfindingNode* currentNode, bool scale, int scaleFactor )
{
	list<SAIFloat3>  nodes;
	SAIFloat3 p = currentNode->Pos;
	if ( scale && scaleFactor > 0 )
	{
		p.x *= scaleFactor;
		p.z *= scaleFactor;
		//ai->utility->ChatMsg("PM: scaling to (%f,%f)", p.x, p.z );
	}
	nodes.push_front( currentNode->Pos );
	vector<SAIFloat3> a;
	a.push_back( p );

	while ( currentNode->CameFrom != NULL )
	{
		p = currentNode->CameFrom->Pos;
		if ( scale && scaleFactor > 0 )
		{
			p.x *= scaleFactor;
			p.z *= scaleFactor;
			//ai->utility->ChatMsg("PM: scaling to (%f,%f)", p.x, p.z );
		}
		//ai->utility->Log( ALL, SLOPEMAP, "(%d, %d) has a predecessor (%d, %d)", currentNode->XIndex, currentNode->ZIndex, currentNode->CameFrom->XIndex, currentNode->CameFrom->ZIndex );
		nodes.push_front( p );
		a.push_back( p );
		currentNode = currentNode->CameFrom;
	}

	return a;
}

void PathfindingMap::DeleteNodes( map<int, PathfindingNode*> closedSet, map<int, PathfindingNode*> openSet )
{
	vector<PathfindingNode*> nodesToDelete;
	map<int, PathfindingNode*>::iterator it;
	

	//ai->utility->ChatMsg("PM: About to delete nodes");
	//ai->utility->ChatMsg("Closed set size: %d", closedSet.size() );
	//ai->utility->ChatMsg("Open set size: %d", openSet.size() );

	//ai->utility->Log( ALL, PATHFIND, "Deleting %d nodes", closedSet.size()+openSet.size() );

	for ( map<int, PathfindingNode*>::iterator it = openSet.begin() ; it != openSet.end() ; it++ )
	{
		if ( it->second != NULL )
			delete it->second;
		//it->second = NULL;
	}
	//ai->utility->ChatMsg("PM: Clearing openset");
	openSet.clear();
	//ai->utility->ChatMsg("PM: Cleared openset");
	
	for ( map<int, PathfindingNode*>::iterator it = closedSet.begin() ; it != closedSet.end() ; it++ )
	{
		if ( it->second != NULL )
			delete it->second;
		//it->second = NULL;
	}
	//ai->utility->ChatMsg("PM: Clearing closedset");
	closedSet.clear();
	//ai->utility->ChatMsg("PM: Cleared closedset");	
}

void PathfindingMap::PrintSection(SAIFloat3 pos)
{
	int x = pos.x/Resolution;
	int z = pos.z/Resolution;
	UnitDef *commanderDef = ai->commander->GetDef();
	MoveData *move = commanderDef->GetMoveData();

	for(int i = max(0,z - 50); i <= min(MapHeight-1, z + 50); i++)
	{
		for(int j = max(0,x - 50); j <= min(MapWidth-1, x + 50); j++)
		{
			bool walkable = MapArray[i*MapWidth + j] < move->GetMaxSlope();
			if(x == j && z == i)
				ai->utility->LogNN(ALL, MISC, (walkable ? "X":"Q"));
			else
				ai->utility->LogNN(ALL, MISC, "%d", walkable);
		}
		ai->utility->LogNN(ALL, MISC, "\n");
	}
	delete commanderDef;
	delete move;
	
}

void brainSpace::PathfindingMap::ResetEntireMap()
{
	Map *map = ai->callback->GetMap();
	//vector<float> slopeMap = map->GetSlopeMap();
	SlopeMap = map->GetSlopeMap();
	for ( int z = 0 ; z < MapHeight ; z++ )
		for ( int x = 0 ; x < MapWidth ; x++ )
			ResetSlope( x, z );
	delete map;
}


Path* PathfindingMap::FindSuperiorPathTo( MilitaryUnitGroup* group, SAIFloat3 start, SAIFloat3 destination, bool debug )
{
	if (debug) ai->utility->Log(ALL, PATHFIND,  "FindPathTo start, resolution: %d",Resolution );
	float ThreatMapRes = ai->knowledge->mapInfo->threatMap->GetMapData()->MapResolution;
	int ThreatMapWidth = ai->knowledge->mapInfo->threatMap->GetMapData()->MapWidth;
	int ThreatMapHeight = ai->knowledge->mapInfo->threatMap->GetMapData()->MapHeight;


	int sourceXIndex = start.x/ThreatMapRes;
	int sourceZIndex = start.z/ThreatMapRes;
	int goalXIndex = destination.x/ThreatMapRes;
	int goalZIndex = destination.z/ThreatMapRes;

	//ai->utility->Log( ALL, PATHFIND, "Source: (%d,%d). Destination: (%d,%d)", sourceXIndex, sourceZIndex, goalXIndex, goalZIndex );
	
	map<int, PathfindingNode*> closedSet;
	map<int, PathfindingNode*> openSet;
	
	Superiority s = ai->knowledge->mapInfo->threatMap->GetImaginarySuperiorityAtPos( start, group, false );
	//ai->utility->ChatMsg("PM: Superiority at source: %d", s );
	if ( s != INFERIOR && s != EQUAL )
	{
		PathfindingNode* startNode = new PathfindingNode( start, sourceXIndex, sourceZIndex, 0, ai->utility->EuclideanDistance( start, destination ), ai->utility->EuclideanDistance( start, destination ), (float)s  );

		openSet.insert( make_pair((int)sourceZIndex*ThreatMapWidth + (int)sourceXIndex, startNode ) );
		if (debug) ai->utility->Log(ALL, PATHFIND,  "FindPathTo inserted first node in openset" );
		if (debug) ai->utility->Log( ALL, PATHFIND, "Starting at tile(%d, %d)", startNode->XIndex, startNode->ZIndex );
	}
	while (!openSet.empty() )
	{
		//if (debug) ai->utility->Log( ALL, PATHFIND, "Openset is not empty" );
		PathfindingNode* current = NULL;
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

		if (current == NULL)
		{
			//if (debug) ai->utility->Log(ALL, PATHFIND,  "FindPathTo Current node is null" );
			vector<SAIFloat3> emptyResult;
			DeleteNodes( closedSet, openSet );
			Path* p = new Path( ai, emptyResult );
			return p;
		}

		/** CHECK FOR GOAL **/
		//if (debug) ai->utility->Log( ALL, PATHFIND, "\nCurrent node(%d, %d)G: %f. H = %f. F = %f", current->XIndex, current->ZIndex, current->Gscore, current->Hscore, current->Fscore );
		if ( current->XIndex == goalXIndex && current->ZIndex == goalZIndex )
		{
		//	if (debug) ai->utility->Log( ALL, PATHFIND, "We have found the goal node" );
			vector<SAIFloat3> shortestPath = ReconstructPath( current );

			DeleteNodes( closedSet, openSet );
			Path* p = new Path(ai, shortestPath );
			return p;
		}

		openSet.erase( currentIndex );
		closedSet.insert( make_pair( current->ZIndex*ThreatMapWidth + current->XIndex, current ) );
		//if (debug) ai->utility->Log( ALL, PATHFIND, "Current (%d,%d) moved to closed set", current->XIndex, current->ZIndex );

		for ( int z = current->ZIndex-1 ; z <= current->ZIndex+1 ; z++ ) //for each neighbour of current node...
		{
			for ( int x = current->XIndex-1 ; x <= current->XIndex+1 ; x++ )
			{
				if ( z < 0 || z > ThreatMapHeight-1 || x < 0 || x > ThreatMapWidth-1 ) 
				{
					//out of bounds check
					if (debug) ai->utility->Log( ALL, PATHFIND, "Neighbour out of bounds" );
					continue;
				}
				if ( z == current->ZIndex && x == current->XIndex ) //skip the current node
				{
					if (debug) ai->utility->Log( ALL, PATHFIND, "Skipping the current node, we are twins" );
					continue;
				}


				map<int, PathfindingNode*>::iterator it = closedSet.find( z*ThreatMapWidth + x );
				if ( it != closedSet.end() )
				{
					if (debug) ai->utility->Log( ALL, PATHFIND, "(%d, %d) was in closedset", it->second->XIndex, it->second->ZIndex );
					continue;
				}
				else if (debug) ai->utility->Log( ALL, PATHFIND, "(%d, %d) was not in closedset", x, z );

				bool neighbourInOpenSet = true;
				it = openSet.find( z*ThreatMapWidth + x );
				if ( it == openSet.end() )
				{
					neighbourInOpenSet = false;
					if (debug) ai->utility->Log( ALL, PATHFIND, "(%d, %d) was NOT in openset", x, z );
				}
				else if (debug) ai->utility->Log( ALL, PATHFIND, "(%d, %d) was in openset", x, z );
					

				SAIFloat3 pos;
				pos.x = x*ThreatMapRes + 0.5*ThreatMapRes;
				pos.z = z*ThreatMapRes + 0.5*ThreatMapRes;
				pos.y = 50;

				float tentativeGScore = current->Gscore + ai->utility->EuclideanDistance( current->Pos, pos );
				if (debug) ai->utility->Log( ALL, PATHFIND, "Tentative g score %f for neighbour (%d, %d)", tentativeGScore, x, z );
				bool tentativeIsBetter = true;
				


				PathfindingNode* neighbour;
				if ( !neighbourInOpenSet )
				{
					neighbour = new PathfindingNode();
					neighbour->Pos = pos;
					//ai->utility->Log( ALL, PATHFIND, "neighbour pos: %f,%f", pos.x,pos.z );
					neighbour->XIndex = x;
					//ai->utility->Log( ALL, PATHFIND, "neighbour x: %d", x);
					neighbour->ZIndex = z;
					neighbour->Slope = (float)ai->knowledge->mapInfo->threatMap->GetImaginarySuperiorityAtPos(pos, group, false );

					//ai->utility->ChatMsg("Neighbour: (%d,%d). Pos: (%f,%f). Slope: %f", x, z, pos.x, pos.z, neighbour->Slope );
					//ai->utility->Log( ALL, PATHFIND, "neighbour z: %d", z);
					if(debug)  ai->utility->Log( ALL, PATHFIND, "neighbour x: %d, z: %d", x, z );
					if ( ( z*ThreatMapWidth + x ) >= ThreatMapWidth*ThreatMapHeight )
						ai->utility->Log( ALL, PATHFIND, "Fuck dig!! %d. MapWidth: %d. MapHeight: %d", z*ThreatMapWidth+x, ThreatMapWidth, ThreatMapHeight );
					
					//ai->utility->Log( ALL, PATHFIND, "mapwidth: %d, mapheight: %d", MapWidth, MapHeight);
					//neighbour->Slope = MapArray[ z*MapWidth + x ];
					if(debug) ai->utility->Log( ALL, PATHFIND, "New neighbour initialised, slope: %f ", neighbour->Slope );
					bool suckyNeighbour = false;
					if(current->ZIndex > neighbour->ZIndex || current->ZIndex < neighbour->ZIndex)
					{
						if(current->XIndex > neighbour->XIndex)
						{
							int tmpX = x + 1;
							if ( tmpX >= 0 && tmpX < ThreatMapWidth )
							{
								SAIFloat3 tmpPos;
								tmpPos.x = tmpX*ThreatMapRes + 0.5f*ThreatMapRes;
								tmpPos.z = z*ThreatMapRes + 0.5f*ThreatMapRes;
								Superiority sup = ai->knowledge->mapInfo->threatMap->GetImaginarySuperiorityAtPos( tmpPos, group, false );
								if( sup == INFERIOR || sup == EQUAL )
								{
									suckyNeighbour = true;
								}
							}
						}
						else if(current->XIndex < neighbour->XIndex)
						{
							int tmpX = x - 1;
							if( tmpX >= 0 && tmpX < ThreatMapWidth )
							{
								SAIFloat3 tmpPos;
								tmpPos.x = tmpX*ThreatMapRes + 0.5f*ThreatMapRes;
								tmpPos.z = z*ThreatMapRes + 0.5f*ThreatMapRes;
								Superiority sup = ai->knowledge->mapInfo->threatMap->GetImaginarySuperiorityAtPos( tmpPos, group, false );
								if( sup == INFERIOR || sup == EQUAL )
								{
									suckyNeighbour = true;
								}
							}
						}
					}
					//if (debug) ai->utility->Log( ALL, PATHFIND, "X check done.. %d", suckyNeighbour );
					if(current->XIndex > neighbour->XIndex || current->XIndex < neighbour->XIndex)
					{
						if(current->ZIndex > neighbour->ZIndex)
						{
							int tmpZ = z + 1;
							if( tmpZ >= 0 && tmpZ < ThreatMapHeight )
							{
								SAIFloat3 tmpPos;
								tmpPos.x = x*ThreatMapRes + 0.5f*ThreatMapRes;
								tmpPos.z = tmpZ*ThreatMapRes + 0.5f*ThreatMapRes;
								Superiority sup = ai->knowledge->mapInfo->threatMap->GetImaginarySuperiorityAtPos( tmpPos, group, false );
								if( sup == INFERIOR || sup == EQUAL )
								{
									suckyNeighbour = true;
								}
							}
						}
						else if(current->ZIndex < neighbour->ZIndex)
						{
							int tmpZ = z - 1;
							if( tmpZ >= 0 && tmpZ < ThreatMapHeight )
							{
								SAIFloat3 tmpPos;
								tmpPos.x = x*ThreatMapRes + 0.5f*ThreatMapRes;
								tmpPos.z = tmpZ*ThreatMapRes + 0.5f*ThreatMapRes;
								Superiority sup = ai->knowledge->mapInfo->threatMap->GetImaginarySuperiorityAtPos( tmpPos, group, false );
								if( sup == INFERIOR || sup == EQUAL )
								{
									suckyNeighbour = true;
								}
							}
						}
					}
					//if (debug) ai->utility->Log( ALL, PATHFIND, "Z check done.. %d", suckyNeighbour );			

					if ( suckyNeighbour )
					{
						closedSet.insert( make_pair( z*ThreatMapWidth + x, neighbour ) );
						tentativeIsBetter = false;
						if (debug) ai->utility->Log( ALL, PATHFIND, "Bad neighbour (%d,%d)", x, z );
						continue;
					}
					else
					{
						if (debug) ai->utility->Log( ALL, PATHFIND, "Good neighbour (%d,%d)", x, z );
						openSet.insert( make_pair( z*ThreatMapWidth + x, neighbour ) );
					}

				}
				else
				{
					if (debug) ai->utility->Log( ALL, PATHFIND, "Already in openset");
					neighbour = it->second;
					if ( tentativeGScore < neighbour->Gscore )
					{
						if (debug) ai->utility->Log( ALL, PATHFIND, "Tentative was better: %f. Neighbour: %f", tentativeGScore, neighbour->Gscore );
						tentativeIsBetter = true;
					}
					else
					{
						tentativeIsBetter = false;
						if (debug) ai->utility->Log( ALL, PATHFIND, "Tentative is worse" );
					}
				}

				if ( tentativeIsBetter )
				{
					//PathfindingNode *n = new PathfindingNode( current.Pos, current.XIndex, current.ZIndex, current.Gscore, current.Hscore, current.Fscore );
					neighbour->CameFrom = current;
					neighbour->Gscore = tentativeGScore;
					neighbour->Hscore = ai->utility->EuclideanDistance( neighbour->Pos, destination );
					neighbour->Fscore = neighbour->Gscore + neighbour->Hscore;
					if (debug) ai->utility->Log( ALL, PATHFIND, "New connection - Current(%d,%d) to Neighbour(%d, %d) G = %f. H = %f. F= %f", neighbour->CameFrom->XIndex, neighbour->CameFrom->ZIndex, neighbour->XIndex, neighbour->ZIndex, neighbour->Gscore, neighbour->Hscore, neighbour->Fscore );	
				}
			}
		}
	}
	if (debug) ai->utility->Log(ALL, PATHFIND, "Openset is empty without finding result");
	vector<SAIFloat3> emptyResult;
	DeleteNodes(closedSet, openSet);
	Path* p = new Path(ai, emptyResult );
	return p;

}
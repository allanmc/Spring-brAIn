
#include "QuadTree.h"

using namespace std;
using namespace springai;
using namespace brainSpace;

QuadTree::QuadTree( AIClasses* aiClasses, CBoundingBox box )
{
	ai = aiClasses,
	RootNode = new QuadTreeNode( ai, box, 0, NULL );
}

QuadTree::~QuadTree()
{
	delete RootNode;
}

bool QuadTree::RemoveUnit( int unitID )
{
	ai->utility->Log(ALL,MISC, "QuadTree::RemoveUnit, units.size: %d", units.size());
	map<int, struct UnitInformationContainer>::iterator it = units.find(unitID);
	if (it == units.end())
	{
		return false;
	}
	ai->utility->Log(ALL,MISC, "QuadTree::RemoveUnit, units.size new: %d", units.size());
	QuadTreeNode *iter = RootNode;

	SAIFloat3 pos = units[unitID].pos;
	while ( true )
	{
		if ( !iter->IsLeafNode() )
		{
			//Let iter point to the childnode and run the while loop again
			iter = iter->GetContainingNode( pos );
			continue;
		}
		//The node has no children. Attempt to delete the unit here.
		else
		{
			ai->utility->Log( LOG_DEBUG, KNOWLEDGE, "Removing unit %d", unitID );
			iter->RemoveUnit(unitID);
			do 
			{
				iter = iter->GetParentNode();
				if (!iter)
					break;
			} while (iter->TryToMergeToLeaf());
			break;
		}
	}
	units.erase(it);
	return true;
}

UnitDef* QuadTree::GetUnitDef(int unitID)
{
	if (units.find(unitID) == units.end())
		return NULL;	
	else
		return units[unitID].def;

}

///@return the amount of units added to the Qtree
int QuadTree::UpdateUnit( int unitID, SAIFloat3 pos )
{
	int i = 0;
	if(!RemoveUnit(unitID))
	{
		i++;
	}
	InsertUnit( unitID, pos );
	return i;
}

SAIFloat3 QuadTree::GetLastUnitPos( int unitID ) 
{
	map<int, UnitInformationContainer>::iterator it = units.find(unitID);
	if (it == units.end())
	{
		return (SAIFloat3){0,0,0};
	} 
	else
	{
		return units[unitID].pos;
	}
}

void QuadTree::InsertUnit( int unitID, SAIFloat3 pos )
{
	QuadTreeNode *iter = RootNode;
	Unit* u = Unit::GetInstance( ai->callback, unitID );
	UnitDef* def = u->GetDef();
	if((pos.x == 0 && pos.z == 0) || def == NULL)
		return;
	//RemoveUnit(unitID);

	if (!QuadTreeNode::IsInsideBoundingBox(pos, RootNode->GetBoundingBox()) )
	{
		ai->utility->Log(LOG_DEBUG, QUADTREE, "QuadTree error: The position is not inside the RootNode... Idiot.");
		return;
	}

	while ( true )
	{

		if ( !iter->IsLeafNode() )
		{

			//Let iter point to the childnode and run the while loop again
			iter = iter->GetContainingNode( pos );
			continue;
		}
		//The node has no children. Attempt to insert the unit here.
		else if ( iter->CheckBucketSize() )
		{
			if ( def == NULL )
			{
				return;
				iter->InsertUnit( unitID, pos );
				units[unitID].def = NULL;
			}
			else
			{
				iter->InsertUnit( unitID, pos, def );
				units[unitID].def = def;
			}
			units[unitID].pos = pos;
			delete u;
			break;
		}
		//The current node was full, so split it.
		else
		{
			//make sure that the split helps (no collisions)
			map<int, struct UnitInformationContainer>::iterator it;

			bool collision = false;
			for ( it = iter->UnitsContained.begin() ; it != iter->UnitsContained.end() ; it++ )
			{
				if (( *it).second.pos.x == pos.x && (*it).second.pos.z == pos.z )
				{
					pos.z += .005f;
					iter = RootNode;
					collision = true;
					break;
				}
			}
			if (collision) continue;


			iter->Split();
			iter->MoveUnitsToChildren();
		}
	}
}

vector<Unit*> QuadTree::RangeQuery(CBoundingBox bbox)
{
	vector<Unit*> units;

	QuadTreeNode* currentNode;
	queue<QuadTreeNode*> nodeQueue;
	nodeQueue.push(RootNode);
	while ( !nodeQueue.empty() )
	{
		currentNode = nodeQueue.front();
		nodeQueue.pop();
		if ( !currentNode->IsLeafNode() )
		{
			for (int i = 0; i < 4; i++)
			{
				if (currentNode->Intersects(bbox))
				{
					nodeQueue.push(currentNode->GetChildAtIndex(i));
				}
			}
			continue;
		}
		else
		{
			map<int, struct UnitInformationContainer>::iterator iter;

			for ( iter = currentNode->UnitsContained.begin() ; iter != currentNode->UnitsContained.end() ; iter++ )
			{
				int unitID = (*iter).first;
				SAIFloat3 pos = (*iter).second.pos;
				if (QuadTreeNode::IsInsideBoundingBox(pos, bbox))
				{
					Unit *u = Unit::GetInstance(ai->callback, unitID);
					units.push_back(u);
				}
			}
		}
	}
	return units;
}

vector<Unit*> QuadTree::RangeQuery(SAIFloat3 topLeft, SAIFloat3 bottomRight)
{
	CBoundingBox bbox;
	bbox.topLeft = topLeft;
	bbox.bottomRight = bottomRight;
	return RangeQuery(bbox);
}

vector<Unit*> QuadTree::RangeQuery(float topLeftX, float topLeftZ, float bottomRightX, float bottomRightZ)
{
	SAIFloat3 topLeft = (SAIFloat3){topLeftX,0,topLeftZ};
	SAIFloat3 bottomRight = (SAIFloat3){bottomRightX,0,bottomRightZ};
	return RangeQuery(topLeft, bottomRight);
}

void QuadTree::Print()
{
	Print(RootNode);
}

void QuadTree::Print( QuadTreeNode* iter)
{
	CBoundingBox box = iter->GetBoundingBox();
	ai->utility->ChatMsg("Level: %d", iter->GetLevel());
	ai->utility->ChatMsg("BBox topleft: ( %f, %f )", box.topLeft.x, box.topLeft.z);
	ai->utility->ChatMsg("BBox bottomright: ( %f, %f )", box.bottomRight.x, box.bottomRight.z);
	ai->utility->ChatMsg("Units contained: %d", iter->GetNumberOfUnits());

	map<int, UnitInformationContainer> pik = iter->UnitsContained;
	for ( map<int, UnitInformationContainer>::iterator it = pik.begin() ; it != pik.end() ; it++ )
	{
		ai->utility->ChatMsg( "UnitID: %d", it->first );
	}
	if ( iter->IsLeafNode() )
		ai->utility->ChatMsg("Current node is leaf");
	else
	{
		for ( int i = 0 ; i < 4 ; i++ )
		{
			Print( iter->GetChildAtIndex(i) );
		}
	}
}


QuadTreeNode* QuadTree::GetRootNode()
{
	return RootNode;
}

const map<int, struct UnitInformationContainer> QuadTree::GetUnits()
{
	return units;
}
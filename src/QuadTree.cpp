
#include "QuadTree.h"

QuadTree::QuadTree( AIClasses* aiClasses, CBoundingBox box )
{
	ai = aiClasses,
	RootNode = new QuadTreeNode( ai, box, 0, NULL );
}

QuadTree::~QuadTree()
{
}

void QuadTree::RemoveUnit( int unitID )
{
	QuadTreeNode *iter = RootNode;
	SAIFloat3 pos = units[unitID];
	while ( true )
	{
		if ( !iter->IsLeafNode() )
		{
			//Let iter point to the childnode and run the while loop again
			iter = iter->GetContainingNode( pos );
			continue;
		}
		//The node has no children. Attempt to insert the unit here.
		else
		{
			iter->RemoveUnit(unitID);
			units.erase(unitID);
			do 
			{
				iter = iter->GetParentNode();
				if (!iter)
					break;
			} while (iter->TryToMergeToLeaf());
			break;
		}

	}
}

void QuadTree::UpdateUnit( int unitID, SAIFloat3 pos )
{
	RemoveUnit( unitID );
	InsertUnit( unitID, pos );
}

SAIFloat3 QuadTree::GetLastUnitPos( int unitID ) 
{
	return units[unitID];
}

void QuadTree::InsertUnit( int unitID, SAIFloat3 pos )
{
	QuadTreeNode *iter = RootNode;

	if (!QuadTreeNode::IsInsideBoundingBox(pos, RootNode->GetBoundingBox()) )
	{
		ai->utility->ChatMsg("QuadTree error: The position is not inside the RootNode... Idiot.");
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
			iter->InsertUnit( unitID, pos );
			units[unitID] = pos;
			break;
		}
		//The current node was full, so split it.
		else
		{
			//make sure that the split helps (no collisions)
			map<int, SAIFloat3>::iterator it;

			bool collision = false;
			for ( it = iter->UnitsContained.begin() ; it != iter->UnitsContained.end() ; it++ )
			{
				if (( *it).second.x == pos.x && (*it).second.z == pos.z )
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
			map<int, SAIFloat3>::iterator iter;

			for ( iter = currentNode->UnitsContained.begin() ; iter != currentNode->UnitsContained.end() ; iter++ )
			{
				int unitID = (*iter).first;
				SAIFloat3 pos = (*iter).second;
				if (QuadTreeNode::IsInsideBoundingBox(pos, bbox))
				{
					units.push_back(Unit::GetInstance(ai->callback, unitID));
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
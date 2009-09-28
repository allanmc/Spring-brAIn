
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
			QuadTreeNode* parent = iter->GetParentNode();
			if (!parent)
				break;
			int count = 0;
			for (int i = 0; i < 4 ; i++)
			{
				count+=iter->GetChildAtIndex(i)->GetNumberOfUnits();
			}
			if (count<=BUCKET_SIZE)
			{
				//Merge
				
			}

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

	if (QuadTreeNode::IsInsideBoundingBox(pos, iter->GetBoundingBox()) )
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

void QuadTree::Print( QuadTreeNode* iter)
{
	CBoundingBox box = iter->GetBoundingBox();
	cout << "Level: " << iter->GetLevel() << endl;
	cout << "BBox topleft: ( " << box.topLeft.x << ", " << box.topLeft.z << ")" << endl;
	cout << "BBox bottomright: ( " << box.bottomRight.x << ", " << box.bottomRight.z << ")" << endl;
	cout << "Units contained: " << iter->GetNumberOfUnits() << endl;
	if ( iter->IsLeafNode() )
		cout << "Current node is leaf" << endl;
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
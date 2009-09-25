#include "QuadTree.h"
#include "QuadTreeNode.h"


QuadTree::QuadTree( CBoundingBox box )
{
	RootNode = new QuadTreeNode( box, 0 );
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
			break;
		}

	}
}

void QuadTree::UpdateUnit( int unitID, SAIFloat3 pos )
{
	RemoveUnit( unitID );
	InsertUnit( unitID, pos );
}

void QuadTree::InsertUnit( int unitID, SAIFloat3 pos )
{
	QuadTreeNode *iter = RootNode;

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
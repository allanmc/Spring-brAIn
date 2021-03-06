#include "QuadTreeNode.h"

using namespace std;
using namespace springai;
using namespace brainSpace;

QuadTreeNode::QuadTreeNode( AIClasses* aiClasses, CBoundingBox box, int level, QuadTreeNode* parentNode )
{
	ai = aiClasses;
	BoundingBox = box;
	IsLeaf = true;
	Level = level;
	parent = parentNode;
	for(int i = 0; i<4; i++)
	{
		Children[i] = NULL;
	}
}

QuadTreeNode::~QuadTreeNode()
{
	for(int i = 0; i<4; i++)
	{
		delete Children[i];
	}
}

QuadTreeNode* QuadTreeNode::GetParentNode()
{
	return parent;
}

bool QuadTreeNode::IsLeafNode()
{
	return IsLeaf;
}

bool QuadTreeNode::Intersects(CBoundingBox bbox)
{
	return BoundingBox.Intersects(bbox);
}

CBoundingBox QuadTreeNode::GetBoundingBox()
{
	return BoundingBox;
}

QuadTreeNode* QuadTreeNode::GetContainingNode( SAIFloat3 pos )
{

	if ( !IsInsideBoundingBox( pos, BoundingBox ) )
	{
		//ai->utility->ChatMsg("x: %f, z: %f, X: %f, Z: %f. Pos: (%f, %f)", BoundingBox.bottomRight.x, BoundingBox.bottomRight.z, BoundingBox.topLeft.x, BoundingBox.topLeft.z, pos.x, pos.z );
		//ai->utility->ChatMsg("PIKSVIN");
		return NULL;
	}
	if ( !IsLeafNode() )
	{
		for ( int i = 0 ; i < 4 ; i++ )
		{
			if ( IsInsideBoundingBox( pos, Children[i]->GetBoundingBox() ) )
			{
				return Children[i];
			}
		}
	}

	return this;
}

bool QuadTreeNode::IsInsideBoundingBox( SAIFloat3 pos, CBoundingBox box )
{
	return 
		( pos.x >= box.topLeft.x &&
		pos.x < box.bottomRight.x &&
		pos.z >= box.topLeft.z &&
		pos.z < box.bottomRight.z );
}

QuadTreeNode* QuadTreeNode::GetChildAtIndex( int i )
{
	if ( i < 0 ||i > 4 )
		return NULL;
	return Children[i];
}

int QuadTreeNode::GetNumberOfUnits()
{
	return UnitsContained.size();
}

void QuadTreeNode::Split()
{
	float middleX, middleZ;
	middleX = (( BoundingBox.bottomRight.x - BoundingBox.topLeft.x )/2.0f) + BoundingBox.topLeft.x;
	middleZ = (( BoundingBox.bottomRight.z - BoundingBox.topLeft.z )/2.0f) + BoundingBox.topLeft.z;

	CBoundingBox boxes[4];


	boxes[0].topLeft = BoundingBox.topLeft;

	boxes[0].bottomRight.x = middleX;
	boxes[0].bottomRight.y = BoundingBox.bottomRight.y;
	boxes[0].bottomRight.z = middleZ;


	boxes[1].topLeft.x = middleX;
	boxes[1].topLeft.y = BoundingBox.topLeft.y;
	boxes[1].topLeft.z = BoundingBox.topLeft.z;

	boxes[1].bottomRight.x = BoundingBox.bottomRight.x;
	boxes[1].bottomRight.y = BoundingBox.bottomRight.y;
	boxes[1].bottomRight.z = middleZ;


	boxes[2].topLeft.x = BoundingBox.topLeft.x;
	boxes[2].topLeft.y = BoundingBox.topLeft.y;
	boxes[2].topLeft.z = middleZ;

	boxes[2].bottomRight.x = middleX;
	boxes[2].bottomRight.y = BoundingBox.bottomRight.y;
	boxes[2].bottomRight.z = BoundingBox.bottomRight.z;


	
	boxes[3].topLeft.x = middleX;
	boxes[3].topLeft.y = BoundingBox.topLeft.y;
	boxes[3].topLeft.z = middleZ;

	boxes[3].bottomRight = BoundingBox.bottomRight;

	for ( int i = 0 ; i < 4 ; i++ )
	{
	
		Children[i] = new QuadTreeNode( ai, boxes[i], Level+1, this );
	}
	IsLeaf = false;
}


bool QuadTreeNode::CheckBucketSize()
{
	if ( UnitsContained.size() +1 > BUCKET_SIZE )
		return false;
	return true;

}

void QuadTreeNode::RemoveUnit( int unitID )
{
	UnitsContained.erase( unitID );
}
void QuadTreeNode::InsertUnit( int unitID, SAIFloat3 pos, UnitDef* def )
{
	map<int, struct UnitInformationContainer>::iterator iter;
	for ( iter = UnitsContained.begin() ; iter != UnitsContained.end() ; iter ++ )
	{
		if (( *iter).second.pos.x == pos.x && (*iter).second.pos.z == pos.z )
		{
			pos.z += 0.0005f;
		}
	}
	struct UnitInformationContainer c;
	c.def = def;
	c.pos = pos;
	UnitsContained[unitID] = c;
}

bool QuadTreeNode::TryToMergeToLeaf()
{
	map<int, struct UnitInformationContainer> innerNodes;

	if (IsLeaf) //Already a leaf?
	{
		ai->utility->Log(ALL, MISC, "QuadTree error: Can't merge a node which already is a leaf.");
		return false;
	}

	//Find every child node and store in innerNodes
	for (int i = 0; i < 4 ; i++)
	{
		if (!Children[i]->IsLeaf) //If this isn't a leafnode, we shouldn't be here.
		{
			//ai->utility->Log(ALL, MISC, "QuadTree error: Only merge from bottom up!");
			return false;
		}
		map<int, struct UnitInformationContainer>::iterator iter;

		for ( iter = Children[i]->UnitsContained.begin() ; iter != Children[i]->UnitsContained.end() ; iter++ )
		{
			int unitID = (*iter).first;
			SAIFloat3 pos = (*iter).second.pos;
			UnitDef* def = iter->second.def;
			innerNodes[unitID].pos = pos;
			innerNodes[unitID].def = def;
		}
	}

	//If the number of collected child nodes is less than or equal to the bucket size,
	//then we need to merge
	int childCount = innerNodes.size();
	if (childCount<=BUCKET_SIZE/2)
	{
		UnitsContained = innerNodes;
		delete Children[0];
		Children[0] = NULL;
		delete Children[1];
		Children[1] = NULL;
		delete Children[2];
		Children[2] = NULL;
		delete Children[3];
		Children[3] = NULL;
		IsLeaf = true;
		return true;
	}
	else
	{
		return false;
	}

}

void QuadTreeNode::MoveUnitsToChildren()
{
	map<int, struct UnitInformationContainer>::iterator iter;

	for ( iter = UnitsContained.begin() ; iter != UnitsContained.end() ; iter++ )
	{
		int unitID = iter->first;
		SAIFloat3 pos = iter->second.pos;
		UnitDef* def = iter->second.def;
		if ( !IsLeaf )
		{
			QuadTreeNode* node = GetContainingNode( pos );
			node->InsertUnit( unitID, pos, def );
		}
	}

	UnitsContained.clear();
}

int QuadTreeNode::GetLevel()
{
	return Level;
}
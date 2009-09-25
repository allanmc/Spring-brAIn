#ifndef _BRAINSPACE_QUADTREENODE_H
#define _BRAINSPACE_QUADTREENODE_H

#include "SAIFloat3.h"
#include "Unit.h"
#include <vector>
#include <map>
#include <iostream>

using namespace std;

#define BUCKET_SIZE 1


typedef struct 
{
	SAIFloat3 topLeft;
	SAIFloat3 bottomRight;

} CBoundingBox;



class QuadTreeNode
{
public:

	QuadTreeNode( CBoundingBox box, int level );

	bool IsLeafNode();

	CBoundingBox GetBoundingBox();

	//Returns a child of this node which contains the given position.
	//Returns itself if the node has no children.
	//Returns NULL if the position is outside the boundingbox of the node.
	QuadTreeNode* GetContainingNode( SAIFloat3 pos );

	QuadTreeNode* GetChildAtIndex( int i );

	int GetNumberOfUnits();

	void Split();

	bool CheckBucketSize();

	void InsertUnit( int unitID, SAIFloat3 pos );
	void RemoveUnit( int unitID );

	void MoveUnitsToChildren();

	int GetLevel();
	
private:

	bool IsInsideBoundingBox( SAIFloat3 pos, CBoundingBox box );

	QuadTreeNode* Children[4];
	CBoundingBox BoundingBox;
	bool IsLeaf;
	int Level;
	map<int, SAIFloat3> UnitsContained;
};


#endif
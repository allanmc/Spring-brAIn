#ifndef _BRAINSPACE_QUADTREE_H
#define _BRAINSPACE_QUADTREE_H


#include "QuadTreeNode.h"


class QuadTree
{
public:
	QuadTree( AIClasses* aiClasses, CBoundingBox box );
	virtual ~QuadTree();

	void InsertUnit( int unitID, SAIFloat3 pos );
	void RemoveUnit( int unitID );
	void UpdateUnit( int unitID, SAIFloat3 pos );
	SAIFloat3 GetLastUnitPos( int unitID );
	QuadTreeNode* GetRootNode();

	void Print( QuadTreeNode* node );

private:
	QuadTreeNode* RootNode;
	map<int, SAIFloat3> units;
	AIClasses *ai;
};

#endif
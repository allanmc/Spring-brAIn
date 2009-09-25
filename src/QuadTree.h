#ifndef _BRAINSPACE_QUADTREE_H
#define _BRAINSPACE_QUADTREE_H

#include "QuadTreeNode.h"

class QuadTree
{
public:
	QuadTree( CBoundingBox box );
	virtual ~QuadTree();

	void InsertUnit( int unitID, SAIFloat3 pos );

	QuadTreeNode* GetRootNode();

	void Print( QuadTreeNode* node );

private:
	QuadTreeNode* RootNode;

};

#endif
#ifndef _BRAINSPACE_QUADTREENODE_H
#define _BRAINSPACE_QUADTREENODE_H

#include "global.h"
#include <vector>
#include <map>
#include <iostream>

using namespace std;

#define BUCKET_SIZE 1

namespace brainSpace
{
	struct CBoundingBox;


	struct CBoundingBox
	{
		SAIFloat3 topLeft;
		SAIFloat3 bottomRight;

		bool Intersects( CBoundingBox box )
		{
			SAIFloat3 min = this->topLeft;
			SAIFloat3 max = this->bottomRight;
			SAIFloat3 other_min = box.topLeft;
			SAIFloat3 other_max = box.bottomRight;

			return 
				(min.x < other_max.x) && (max.x > other_min.x) &&
				/*(min.y < other_max.y) && (max.y > other_min.y) &&*/
				(min.z < other_max.z) && (max.z > other_min.z);
		}

	};



	class QuadTreeNode
	{
	public:

		QuadTreeNode( AIClasses* aiClasses, CBoundingBox box, int level, QuadTreeNode* parentNode );

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
		bool TryToMergeToLeaf();

		int GetLevel();

		map<int, SAIFloat3> UnitsContained;

		static bool IsInsideBoundingBox( SAIFloat3 pos, CBoundingBox box );
		bool Intersects(CBoundingBox bbox);

		QuadTreeNode* GetParentNode();

	private:


		QuadTreeNode* Children[4];
		CBoundingBox BoundingBox;
		bool IsLeaf;
		int Level;
		QuadTreeNode* parent;
		AIClasses *ai;
	};

}
#endif
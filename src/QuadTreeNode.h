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

	///A Box which sides are paralell with the axes of the coodinate system
	/**
	 * The Bounding box consists of two SAIFloat3, but is only used for 2D. (Using the X and Z axes, the Y axis is ignored)
	 */
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

	///A node in the QuadTree, either a branch or a leaf node.
	/**
	 * @see QuadTree
	 */
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

		void InsertUnit( int unitID, SAIFloat3 pos, springai::UnitDef* def = NULL );
		void RemoveUnit( int unitID );

		void MoveUnitsToChildren();
		bool TryToMergeToLeaf();

		int GetLevel();

		map<int, struct UnitInformationContainer> UnitsContained;

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
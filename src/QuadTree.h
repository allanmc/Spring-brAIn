#ifndef _BRAINSPACE_QUADTREE_H
#define _BRAINSPACE_QUADTREE_H


#include "QuadTreeNode.h"


namespace brainSpace{

	class QuadTree
	{
	public:
		QuadTree( AIClasses* aiClasses, CBoundingBox box );
		virtual ~QuadTree();

		void InsertUnit( int unitID, SAIFloat3 pos );
		bool RemoveUnit( int unitID );
		int UpdateUnit( int unitID, SAIFloat3 pos );
		SAIFloat3 GetLastUnitPos( int unitID );
		QuadTreeNode* GetRootNode();
		void Print();
		vector<Unit*> RangeQuery(float topLeftX, float topLeftZ, float bottomRightX, float bottomRightZ);
		vector<Unit*> RangeQuery(SAIFloat3 topLeft, SAIFloat3 bottomRight);
		vector<Unit*> RangeQuery(CBoundingBox bbox);
		void Print( QuadTreeNode* node );
		const map<int, SAIFloat3> GetEnemyUnits();
	private:
		QuadTreeNode* RootNode;
		map<int, SAIFloat3> units;
		AIClasses *ai;
	};
}

#endif
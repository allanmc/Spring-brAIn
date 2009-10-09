#ifndef _BRAINSPACE_QUADTREE_H
#define _BRAINSPACE_QUADTREE_H

#include "Battle.h"
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
		std::vector<springai::Unit*> RangeQuery(float topLeftX, float topLeftZ, float bottomRightX, float bottomRightZ);
		std::vector<springai::Unit*> RangeQuery(SAIFloat3 topLeft, SAIFloat3 bottomRight);
		std::vector<springai::Unit*> RangeQuery(CBoundingBox bbox);
		void Print( QuadTreeNode* node );
		const std::map<int, struct UnitInformationContainer> GetUnits();
	private:
		QuadTreeNode* RootNode;
		std::map<int, struct UnitInformationContainer> units;
		AIClasses *ai;
	};
}

#endif
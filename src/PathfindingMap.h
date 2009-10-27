#ifndef _BRAINSPACE_PATHFINDINGMAP_H
#define _BRAINSPACE_PATHFINDINGMAP_H

#include "types.h"
#include "BrainMap.h"

namespace brainSpace {

	struct PathfindingNode
	{
		SAIFloat3 Pos;
		int XIndex, ZIndex;
		float Gscore, Hscore, Fscore;
		PathfindingNode* CameFrom;
		float Slope;

		PathfindingNode()
		{
		}

		PathfindingNode( SAIFloat3 pos, int xIndex, int zIndex, float g, float h, float f, float slope )
		{
			Pos = pos;
			XIndex = xIndex;
			ZIndex = zIndex;
			Gscore = g;
			Hscore = h;
			Fscore = f;
			CameFrom = NULL;
			Slope = slope;
		}

		bool operator==( PathfindingNode n1 )
		{
			return (
				n1.XIndex == XIndex &&
				n1.ZIndex == ZIndex );
		}



	};

	class PathfindingMap : public BrainMap
	{
	public:


		PathfindingMap( AIClasses* aiClasses );
		
		virtual ~PathfindingMap();

		void Update();

		void AddBuilding( springai::Unit* unit );

		void RemoveBuilding( springai::Unit* unit );

		std::vector<PathfindingNode*> FindPathTo( springai::UnitDef* pathfinder, SAIFloat3 source, SAIFloat3 destination );

		bool IsPossibleToEscapeFrom( springai::UnitDef* pathfinder, springai::UnitDef* building, SAIFloat3 buildPosition, SAIFloat3 escapeFrom, SAIFloat3 escapeTo );

	private:

		void RemoveHypotheticalBuilding(springai::UnitDef* unit, SAIFloat3 pos);

		void AddHypotheticalBuilding(springai::UnitDef* unit, SAIFloat3 pos);

		std::vector<PathfindingNode*> ReconstructPath( PathfindingNode* currentNode );

		void EffectCell(int index, float value);

		void ResetSlope( int xTile, int zTile );

		void DeleteUnusedPathfindingNodes( std::map<int, PathfindingNode*> closedSet, std::map<int, PathfindingNode*> openSet, std::vector<PathfindingNode*> shortestPath );

		std::vector<float> SlopeMap;
		std::map<int,float> backUp;
	};
}

#endif
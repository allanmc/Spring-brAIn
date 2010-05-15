#ifndef _BRAINSPACE_SCENARIO_H
#define _BRAINSPACE_SCENARIO_H


#include "types.h"

#define SCENARIO_MAX_GROUPS 5
#define SCENARIO_MEX_CLUSTERS 6
#define SCENARIO_MEX_CLUSTER_SIZE_MIN 2
#define SCENARIO_MEX_CLUSTER_SIZE_MAX 5

#define INITIAL_SCENARIO_DELAY 5000
#define SCENARIO_DELAY 800
#define CLUSTER_DISTANCE 550

namespace brainSpace {
	
	class Scenario
	{
	public:
		Scenario( AIClasses* aiClasses );
		virtual ~Scenario();

	private:

		void MakeMexCluster( unsigned int size );

		std::vector<SAIFloat3> mexClusterPositions;
		AIClasses* ai;
		std::vector<std::vector<int> > units_all;
		std::vector< std::vector<SAIFloat3> > metalSpotClusters;
		bool IsMetalMap;
	};

}
#endif
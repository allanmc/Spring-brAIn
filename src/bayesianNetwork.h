#ifndef _BAYESIANNETWORK_H
#define _BAYESIANNETWORK_H

#include "global.h"

#include "dlib/bayes_utils.h"
#include "dlib/graph_utils.h"
#include "dlib/graph.h"
#include "dlib/directed_graph.h"
#include <iostream>


namespace brainSpace {

	typedef dlib::graph<dlib::set<unsigned long>::compare_1b_c,dlib::set<unsigned long>::compare_1b_c>::kernel_1a_c join_tree_type;

	enum nodes
	{
		node_seenUnits = 0,
		node_myStrategy = 1,
		node_seenRes = 2,
		node_seenProd = 3,
		node_attacks = 4,
		node_seenDef = 5,
		node_enemyResource = 6,
		node_enemyProduction = 7,
		node_enemyDefence = 8,
		node_enemyUnits = 9,
		node_enemyStrategy = 10
	};

	
	class BayesianNetwork {
		public:
			BayesianNetwork( AIClasses *aiClasses );
			~BayesianNetwork();
			float getBelief(const char* nodeName, const char* stateName);
			void setEvidence(const char* nodeName, const char* stateName);
			void print_beliefs_and_utilities ();
		
		private:
			AIClasses *ai;


	};
}
#endif // _BAYESIANNETWORK_H
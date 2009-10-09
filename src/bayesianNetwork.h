#ifndef _BAYESIANNETWORK_H
#define _BAYESIANNETWORK_H

#include "global.h"

#include "dlib/bayes_utils.h"
#include "dlib/graph_utils.h"
#include "dlib/graph.h"
#include "dlib/directed_graph.h"
#include <iostream>



namespace brainSpace {

	//using namespace dlib;
	//using namespace brainSpace;
	//using namespace bayes_node_utils;
	
	#include "../data/uber_enums.h"
	
	typedef dlib::graph<dlib::set<unsigned long>::compare_1b_c,dlib::set<unsigned long>::compare_1b_c>::kernel_1a_c join_tree_type;

	class BayesianNetwork {
		public:
			BayesianNetwork( AIClasses *aiClasses );
			~BayesianNetwork();
			float getBelief(int node, int state);
			void setEvidence(int node, int state);
			void print_beliefs_and_utilities ();
		
		private:
			AIClasses *ai;
			dlib::bayesian_network_join_tree *solution;
			join_tree_type join_tree;
			dlib::directed_graph<dlib::bayes_node>::kernel_1a_c bn;
	};

}
#endif // _BAYESIANNETWORK_H
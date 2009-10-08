#ifndef _BAYESIANNETWORK_H
#define _BAYESIANNETWORK_H

//#include "dlib/bayes_utils.h"
#include "dlib/graph_utils.h"
#include "dlib/graph.h"
#include "dlib/directed_graph.h"

//static AIClasses *ai;

//#include "global.h"
//using namespace brainSpace;

class BayesianNetwork {
	public:
		BayesianNetwork();
		~BayesianNetwork();
		float getBelief(const char* nodeName, const char* stateName);
		void setEvidence(const char* nodeName, const char* stateName);
		void print_beliefs_and_utilities ();
	
	private:
		

};

#endif // _BAYESIANNETWORK_H
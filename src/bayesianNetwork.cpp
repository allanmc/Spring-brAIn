#include "bayesianNetwork.h"


using namespace dlib;
using namespace brainSpace;
using namespace bayes_node_utils;

BayesianNetwork::BayesianNetwork( AIClasses *aiClasses ) 
{
	ai = aiClasses;
	ai->utility->Log(ALL, MISC, "Loading BN...");
	#include "uber.h"
	ai->utility->Log(ALL, MISC, "uber.h included...");
	create_moral_graph(bn, join_tree);
	ai->utility->Log(ALL, MISC, "Created moral_graph...");
	create_join_tree(join_tree, join_tree);
	ai->utility->Log(ALL, MISC, "Created join_tree...");
	solution = new bayesian_network_join_tree(bn, join_tree);
	ai->utility->Log(ALL, MISC, "Created solution...");
}

BayesianNetwork::~BayesianNetwork()
{
	delete solution;
	solution = NULL;
}

void BayesianNetwork::Propagate()
{
	delete(solution);
	solution = new bayesian_network_join_tree(bn, join_tree);
}

float BayesianNetwork::getBelief(int node, int state)
{
	return solution->probability(node)(state);
}
void BayesianNetwork::setEvidence(int node, int state)
{
	set_node_value(bn, node, state);
    set_node_as_evidence(bn, node);
}
void BayesianNetwork::print_beliefs_and_utilities ()
{
	
}
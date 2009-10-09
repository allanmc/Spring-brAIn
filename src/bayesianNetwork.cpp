#include "bayesianNetwork.h"


using namespace dlib;
using namespace brainSpace;
using namespace bayes_node_utils;

BayesianNetwork::BayesianNetwork( AIClasses *aiClasses ) 
{
	ai = aiClasses;

	#include "../data/uber.h"

	create_moral_graph(bn, join_tree);
	create_join_tree(join_tree, join_tree);
	
	solution = new bayesian_network_join_tree(bn, join_tree);
}

BayesianNetwork::~BayesianNetwork()
{
	delete(solution);
}

float BayesianNetwork::getBelief(int node, int state)
{
	return solution->probability(node)(state);
}
void BayesianNetwork::setEvidence(int node, int state)
{
	set_node_value(bn, node, state);
    set_node_as_evidence(bn, node);
	delete(solution);
	solution = new bayesian_network_join_tree(bn, join_tree);
}
void BayesianNetwork::print_beliefs_and_utilities ()
{
	
}
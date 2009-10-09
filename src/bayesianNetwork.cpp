#include "bayesianNetwork.h"


using namespace dlib;
using namespace brainSpace;
using namespace bayes_node_utils;

BayesianNetwork::BayesianNetwork( AIClasses *aiClasses ) 
{
	ai = aiClasses;

	directed_graph<bayes_node>::kernel_1a_c bn;

	#include "../data/uber.c"


	join_tree_type join_tree;

	create_moral_graph(bn, join_tree);
	create_join_tree(join_tree, join_tree);

	bayesian_network_join_tree solution(bn, join_tree);

}

BayesianNetwork::~BayesianNetwork()
{

}

float BayesianNetwork::getBelief(const char* nodeName, const char* stateName)
{

}
void BayesianNetwork::setEvidence(const char* nodeName, const char* stateName)
{

}
void BayesianNetwork::print_beliefs_and_utilities ()
{

}
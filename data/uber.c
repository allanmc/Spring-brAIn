directed_graph::kernel_1a_c bn;

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
}

bn.set_number_of_nodes(11);
bn.add_edge(node_enemyUnits, node_seenUnits);
bn.add_edge(node_enemyResource, node_seenRes);
bn.add_edge(node_enemyProduction, node_seenProd);
bn.add_edge(node_enemyUnits, node_attacks);
bn.add_edge(node_enemyStrategy, node_attacks);
bn.add_edge(node_enemyDefence, node_seenDef);
bn.add_edge(node_enemyStrategy, node_enemyResource);
bn.add_edge(node_enemyResource, node_enemyProduction);
bn.add_edge(node_enemyStrategy, node_enemyProduction);
bn.add_edge(node_enemyResource, node_enemyDefence);
bn.add_edge(node_enemyStrategy, node_enemyDefence);
bn.add_edge(node_enemyResource, node_enemyUnits);
bn.add_edge(node_enemyProduction, node_enemyUnits);
bn.add_edge(node_enemyStrategy, node_enemyUnits);
bn.add_edge(node_myStrategy, node_enemyStrategy);


set_node_num_values(bn, node_seenUnits, 4);
set_node_num_values(bn, node_myStrategy, 2);
set_node_num_values(bn, node_seenRes, 4);
set_node_num_values(bn, node_seenProd, 4);
set_node_num_values(bn, node_attacks, 4);
set_node_num_values(bn, node_seenDef, 4);
set_node_num_values(bn, node_enemyResource, 4);
set_node_num_values(bn, node_enemyProduction, 4);
set_node_num_values(bn, node_enemyDefence, 4);
set_node_num_values(bn, node_enemyUnits, 4);
set_node_num_values(bn, node_enemyStrategy, 2);


assignment parent_state;

parent_state.clear();
parent_state.add(node_enemyUnits, 0);
set_node_probability(bn, node_seenUnits, 0, parent_state, 1)//state = 0;
set_node_probability(bn, node_seenUnits, 1, parent_state, 0)//state = 1-10;
set_node_probability(bn, node_seenUnits, 2, parent_state, 0)//state = 11-50;
set_node_probability(bn, node_seenUnits, 3, parent_state, 0)//state = 51-300;

parent_state.clear();
parent_state.add(node_enemyUnits, 1);
set_node_probability(bn, node_seenUnits, 0, parent_state, 0.5)//state = 0;
set_node_probability(bn, node_seenUnits, 1, parent_state, 0.5)//state = 1-10;
set_node_probability(bn, node_seenUnits, 2, parent_state, 0)//state = 11-50;
set_node_probability(bn, node_seenUnits, 3, parent_state, 0)//state = 51-300;

parent_state.clear();
parent_state.add(node_enemyUnits, 2);
set_node_probability(bn, node_seenUnits, 0, parent_state, 0.2)//state = 0;
set_node_probability(bn, node_seenUnits, 1, parent_state, 0.5)//state = 1-10;
set_node_probability(bn, node_seenUnits, 2, parent_state, 0.3)//state = 11-50;
set_node_probability(bn, node_seenUnits, 3, parent_state, 0)//state = 51-300;

parent_state.clear();
parent_state.add(node_enemyUnits, 3);
set_node_probability(bn, node_seenUnits, 0, parent_state, 0.1)//state = 0;
set_node_probability(bn, node_seenUnits, 1, parent_state, 0.2)//state = 1-10;
set_node_probability(bn, node_seenUnits, 2, parent_state, 0.4)//state = 11-50;
set_node_probability(bn, node_seenUnits, 3, parent_state, 0.3)//state = 51-300;

set_node_probability(bn, node_myStrategy, 0, parent_state, 0.5)//state = Aggressive;
set_node_probability(bn, node_myStrategy, 1, parent_state, 0.5)//state = Defensive;

parent_state.clear();
parent_state.add(node_enemyResource, 0);
set_node_probability(bn, node_seenRes, 0, parent_state, 1)//state = 0;
set_node_probability(bn, node_seenRes, 1, parent_state, 0)//state = 1-10;
set_node_probability(bn, node_seenRes, 2, parent_state, 0)//state = 11-20;
set_node_probability(bn, node_seenRes, 3, parent_state, 0)//state = 21-40;

parent_state.clear();
parent_state.add(node_enemyResource, 1);
set_node_probability(bn, node_seenRes, 0, parent_state, 0.5)//state = 0;
set_node_probability(bn, node_seenRes, 1, parent_state, 0.5)//state = 1-10;
set_node_probability(bn, node_seenRes, 2, parent_state, 0)//state = 11-20;
set_node_probability(bn, node_seenRes, 3, parent_state, 0)//state = 21-40;

parent_state.clear();
parent_state.add(node_enemyResource, 2);
set_node_probability(bn, node_seenRes, 0, parent_state, 0.166667)//state = 0;
set_node_probability(bn, node_seenRes, 1, parent_state, 0.333333)//state = 1-10;
set_node_probability(bn, node_seenRes, 2, parent_state, 0.5)//state = 11-20;
set_node_probability(bn, node_seenRes, 3, parent_state, 0)//state = 21-40;

parent_state.clear();
parent_state.add(node_enemyResource, 3);
set_node_probability(bn, node_seenRes, 0, parent_state, 0.1)//state = 0;
set_node_probability(bn, node_seenRes, 1, parent_state, 0.2)//state = 1-10;
set_node_probability(bn, node_seenRes, 2, parent_state, 0.4)//state = 11-20;
set_node_probability(bn, node_seenRes, 3, parent_state, 0.3)//state = 21-40;

parent_state.clear();
parent_state.add(node_enemyProduction, 0);
set_node_probability(bn, node_seenProd, 0, parent_state, 1)//state = 0;
set_node_probability(bn, node_seenProd, 1, parent_state, 0)//state = 1-2;
set_node_probability(bn, node_seenProd, 2, parent_state, 0)//state = 3-6;
set_node_probability(bn, node_seenProd, 3, parent_state, 0)//state = 7-10;

parent_state.clear();
parent_state.add(node_enemyProduction, 1);
set_node_probability(bn, node_seenProd, 0, parent_state, 0.7)//state = 0;
set_node_probability(bn, node_seenProd, 1, parent_state, 0.3)//state = 1-2;
set_node_probability(bn, node_seenProd, 2, parent_state, 0)//state = 3-6;
set_node_probability(bn, node_seenProd, 3, parent_state, 0)//state = 7-10;

parent_state.clear();
parent_state.add(node_enemyProduction, 2);
set_node_probability(bn, node_seenProd, 0, parent_state, 0.2)//state = 0;
set_node_probability(bn, node_seenProd, 1, parent_state, 0.4)//state = 1-2;
set_node_probability(bn, node_seenProd, 2, parent_state, 0.4)//state = 3-6;
set_node_probability(bn, node_seenProd, 3, parent_state, 0)//state = 7-10;

parent_state.clear();
parent_state.add(node_enemyProduction, 3);
set_node_probability(bn, node_seenProd, 0, parent_state, 0.1)//state = 0;
set_node_probability(bn, node_seenProd, 1, parent_state, 0.2)//state = 1-2;
set_node_probability(bn, node_seenProd, 2, parent_state, 0.4)//state = 3-6;
set_node_probability(bn, node_seenProd, 3, parent_state, 0.3)//state = 7-10;

parent_state.clear();
parent_state.add(node_enemyUnits, 0);
parent_state.add(node_enemyStrategy, 0);
set_node_probability(bn, node_attacks, 0, parent_state, 1)//state = 0;
set_node_probability(bn, node_attacks, 1, parent_state, 0)//state = 1-3;
set_node_probability(bn, node_attacks, 2, parent_state, 0)//state = 4-8;
set_node_probability(bn, node_attacks, 3, parent_state, 0)//state = 9-;

parent_state.clear();
parent_state.add(node_enemyUnits, 0);
parent_state.add(node_enemyStrategy, 1);
set_node_probability(bn, node_attacks, 0, parent_state, 1)//state = 0;
set_node_probability(bn, node_attacks, 1, parent_state, 0)//state = 1-3;
set_node_probability(bn, node_attacks, 2, parent_state, 0)//state = 4-8;
set_node_probability(bn, node_attacks, 3, parent_state, 0)//state = 9-;

parent_state.clear();
parent_state.add(node_enemyUnits, 1);
parent_state.add(node_enemyStrategy, 0);
set_node_probability(bn, node_attacks, 0, parent_state, 0.1)//state = 0;
set_node_probability(bn, node_attacks, 1, parent_state, 0.5)//state = 1-3;
set_node_probability(bn, node_attacks, 2, parent_state, 0.25)//state = 4-8;
set_node_probability(bn, node_attacks, 3, parent_state, 0.25)//state = 9-;

parent_state.clear();
parent_state.add(node_enemyUnits, 1);
parent_state.add(node_enemyStrategy, 1);
set_node_probability(bn, node_attacks, 0, parent_state, 0.5)//state = 0;
set_node_probability(bn, node_attacks, 1, parent_state, 0.3)//state = 1-3;
set_node_probability(bn, node_attacks, 2, parent_state, 0.1)//state = 4-8;
set_node_probability(bn, node_attacks, 3, parent_state, 0.1)//state = 9-;

parent_state.clear();
parent_state.add(node_enemyUnits, 2);
parent_state.add(node_enemyStrategy, 0);
set_node_probability(bn, node_attacks, 0, parent_state, 0.1)//state = 0;
set_node_probability(bn, node_attacks, 1, parent_state, 0.3)//state = 1-3;
set_node_probability(bn, node_attacks, 2, parent_state, 0.4)//state = 4-8;
set_node_probability(bn, node_attacks, 3, parent_state, 0.2)//state = 9-;

parent_state.clear();
parent_state.add(node_enemyUnits, 2);
parent_state.add(node_enemyStrategy, 1);
set_node_probability(bn, node_attacks, 0, parent_state, 0.34)//state = 0;
set_node_probability(bn, node_attacks, 1, parent_state, 0.6)//state = 1-3;
set_node_probability(bn, node_attacks, 2, parent_state, 0.05)//state = 4-8;
set_node_probability(bn, node_attacks, 3, parent_state, 0.01)//state = 9-;

parent_state.clear();
parent_state.add(node_enemyUnits, 3);
parent_state.add(node_enemyStrategy, 0);
set_node_probability(bn, node_attacks, 0, parent_state, 0.1)//state = 0;
set_node_probability(bn, node_attacks, 1, parent_state, 0.2)//state = 1-3;
set_node_probability(bn, node_attacks, 2, parent_state, 0.5)//state = 4-8;
set_node_probability(bn, node_attacks, 3, parent_state, 0.2)//state = 9-;

parent_state.clear();
parent_state.add(node_enemyUnits, 3);
parent_state.add(node_enemyStrategy, 1);
set_node_probability(bn, node_attacks, 0, parent_state, 0.2)//state = 0;
set_node_probability(bn, node_attacks, 1, parent_state, 0.65)//state = 1-3;
set_node_probability(bn, node_attacks, 2, parent_state, 0.1)//state = 4-8;
set_node_probability(bn, node_attacks, 3, parent_state, 0.05)//state = 9-;

parent_state.clear();
parent_state.add(node_enemyDefence, 0);
set_node_probability(bn, node_seenDef, 0, parent_state, 1)//state = 0;
set_node_probability(bn, node_seenDef, 1, parent_state, 0)//state = 1-5;
set_node_probability(bn, node_seenDef, 2, parent_state, 0)//state = 6-10;
set_node_probability(bn, node_seenDef, 3, parent_state, 0)//state = 11-20;

parent_state.clear();
parent_state.add(node_enemyDefence, 1);
set_node_probability(bn, node_seenDef, 0, parent_state, 0.6)//state = 0;
set_node_probability(bn, node_seenDef, 1, parent_state, 0.4)//state = 1-5;
set_node_probability(bn, node_seenDef, 2, parent_state, 0)//state = 6-10;
set_node_probability(bn, node_seenDef, 3, parent_state, 0)//state = 11-20;

parent_state.clear();
parent_state.add(node_enemyDefence, 2);
set_node_probability(bn, node_seenDef, 0, parent_state, 0.4)//state = 0;
set_node_probability(bn, node_seenDef, 1, parent_state, 0.5)//state = 1-5;
set_node_probability(bn, node_seenDef, 2, parent_state, 0.1)//state = 6-10;
set_node_probability(bn, node_seenDef, 3, parent_state, 0)//state = 11-20;

parent_state.clear();
parent_state.add(node_enemyDefence, 3);
set_node_probability(bn, node_seenDef, 0, parent_state, 0.111111)//state = 0;
set_node_probability(bn, node_seenDef, 1, parent_state, 0.111111)//state = 1-5;
set_node_probability(bn, node_seenDef, 2, parent_state, 0.222222)//state = 6-10;
set_node_probability(bn, node_seenDef, 3, parent_state, 0.555556)//state = 11-20;

parent_state.clear();
parent_state.add(node_enemyStrategy, 0);
set_node_probability(bn, node_enemyResource, 0, parent_state, 0.05)//state = 0;
set_node_probability(bn, node_enemyResource, 1, parent_state, 0.2)//state = 1-10;
set_node_probability(bn, node_enemyResource, 2, parent_state, 0.4)//state = 11-20;
set_node_probability(bn, node_enemyResource, 3, parent_state, 0.35)//state = 21-40;

parent_state.clear();
parent_state.add(node_enemyStrategy, 1);
set_node_probability(bn, node_enemyResource, 0, parent_state, 0.05)//state = 0;
set_node_probability(bn, node_enemyResource, 1, parent_state, 0.3)//state = 1-10;
set_node_probability(bn, node_enemyResource, 2, parent_state, 0.4)//state = 11-20;
set_node_probability(bn, node_enemyResource, 3, parent_state, 0.25)//state = 21-40;

parent_state.clear();
parent_state.add(node_enemyResource, 0);
parent_state.add(node_enemyStrategy, 0);
set_node_probability(bn, node_enemyProduction, 0, parent_state, 0.7)//state = 0;
set_node_probability(bn, node_enemyProduction, 1, parent_state, 0.2)//state = 1-2;
set_node_probability(bn, node_enemyProduction, 2, parent_state, 0.1)//state = 3-6;
set_node_probability(bn, node_enemyProduction, 3, parent_state, 0)//state = 7-10;

parent_state.clear();
parent_state.add(node_enemyResource, 0);
parent_state.add(node_enemyStrategy, 1);
set_node_probability(bn, node_enemyProduction, 0, parent_state, 0.8)//state = 0;
set_node_probability(bn, node_enemyProduction, 1, parent_state, 0.15)//state = 1-2;
set_node_probability(bn, node_enemyProduction, 2, parent_state, 0.05)//state = 3-6;
set_node_probability(bn, node_enemyProduction, 3, parent_state, 0)//state = 7-10;

parent_state.clear();
parent_state.add(node_enemyResource, 1);
parent_state.add(node_enemyStrategy, 0);
set_node_probability(bn, node_enemyProduction, 0, parent_state, 0.3)//state = 0;
set_node_probability(bn, node_enemyProduction, 1, parent_state, 0.5)//state = 1-2;
set_node_probability(bn, node_enemyProduction, 2, parent_state, 0.15)//state = 3-6;
set_node_probability(bn, node_enemyProduction, 3, parent_state, 0.05)//state = 7-10;

parent_state.clear();
parent_state.add(node_enemyResource, 1);
parent_state.add(node_enemyStrategy, 1);
set_node_probability(bn, node_enemyProduction, 0, parent_state, 0.444444)//state = 0;
set_node_probability(bn, node_enemyProduction, 1, parent_state, 0.444444)//state = 1-2;
set_node_probability(bn, node_enemyProduction, 2, parent_state, 0.0555556)//state = 3-6;
set_node_probability(bn, node_enemyProduction, 3, parent_state, 0.0555556)//state = 7-10;

parent_state.clear();
parent_state.add(node_enemyResource, 2);
parent_state.add(node_enemyStrategy, 0);
set_node_probability(bn, node_enemyProduction, 0, parent_state, 0.1)//state = 0;
set_node_probability(bn, node_enemyProduction, 1, parent_state, 0.5)//state = 1-2;
set_node_probability(bn, node_enemyProduction, 2, parent_state, 0.3)//state = 3-6;
set_node_probability(bn, node_enemyProduction, 3, parent_state, 0.1)//state = 7-10;

parent_state.clear();
parent_state.add(node_enemyResource, 2);
parent_state.add(node_enemyStrategy, 1);
set_node_probability(bn, node_enemyProduction, 0, parent_state, 0.2)//state = 0;
set_node_probability(bn, node_enemyProduction, 1, parent_state, 0.6)//state = 1-2;
set_node_probability(bn, node_enemyProduction, 2, parent_state, 0.15)//state = 3-6;
set_node_probability(bn, node_enemyProduction, 3, parent_state, 0.05)//state = 7-10;

parent_state.clear();
parent_state.add(node_enemyResource, 3);
parent_state.add(node_enemyStrategy, 0);
set_node_probability(bn, node_enemyProduction, 0, parent_state, 0.05)//state = 0;
set_node_probability(bn, node_enemyProduction, 1, parent_state, 0.25)//state = 1-2;
set_node_probability(bn, node_enemyProduction, 2, parent_state, 0.5)//state = 3-6;
set_node_probability(bn, node_enemyProduction, 3, parent_state, 0.2)//state = 7-10;

parent_state.clear();
parent_state.add(node_enemyResource, 3);
parent_state.add(node_enemyStrategy, 1);
set_node_probability(bn, node_enemyProduction, 0, parent_state, 0.1)//state = 0;
set_node_probability(bn, node_enemyProduction, 1, parent_state, 0.4)//state = 1-2;
set_node_probability(bn, node_enemyProduction, 2, parent_state, 0.45)//state = 3-6;
set_node_probability(bn, node_enemyProduction, 3, parent_state, 0.05)//state = 7-10;

parent_state.clear();
parent_state.add(node_enemyResource, 0);
parent_state.add(node_enemyStrategy, 0);
set_node_probability(bn, node_enemyDefence, 0, parent_state, 0.9)//state = 0;
set_node_probability(bn, node_enemyDefence, 1, parent_state, 0.04)//state = 1-5;
set_node_probability(bn, node_enemyDefence, 2, parent_state, 0.01)//state = 6-10;
set_node_probability(bn, node_enemyDefence, 3, parent_state, 0.05)//state = 11-20;

parent_state.clear();
parent_state.add(node_enemyResource, 0);
parent_state.add(node_enemyStrategy, 1);
set_node_probability(bn, node_enemyDefence, 0, parent_state, 0.55)//state = 0;
set_node_probability(bn, node_enemyDefence, 1, parent_state, 0.3)//state = 1-5;
set_node_probability(bn, node_enemyDefence, 2, parent_state, 0.1)//state = 6-10;
set_node_probability(bn, node_enemyDefence, 3, parent_state, 0.05)//state = 11-20;

parent_state.clear();
parent_state.add(node_enemyResource, 1);
parent_state.add(node_enemyStrategy, 0);
set_node_probability(bn, node_enemyDefence, 0, parent_state, 0.75)//state = 0;
set_node_probability(bn, node_enemyDefence, 1, parent_state, 0.15)//state = 1-5;
set_node_probability(bn, node_enemyDefence, 2, parent_state, 0.05)//state = 6-10;
set_node_probability(bn, node_enemyDefence, 3, parent_state, 0.05)//state = 11-20;

parent_state.clear();
parent_state.add(node_enemyResource, 1);
parent_state.add(node_enemyStrategy, 1);
set_node_probability(bn, node_enemyDefence, 0, parent_state, 0.1)//state = 0;
set_node_probability(bn, node_enemyDefence, 1, parent_state, 0.4)//state = 1-5;
set_node_probability(bn, node_enemyDefence, 2, parent_state, 0.3)//state = 6-10;
set_node_probability(bn, node_enemyDefence, 3, parent_state, 0.2)//state = 11-20;

parent_state.clear();
parent_state.add(node_enemyResource, 2);
parent_state.add(node_enemyStrategy, 0);
set_node_probability(bn, node_enemyDefence, 0, parent_state, 0.65)//state = 0;
set_node_probability(bn, node_enemyDefence, 1, parent_state, 0.2)//state = 1-5;
set_node_probability(bn, node_enemyDefence, 2, parent_state, 0.1)//state = 6-10;
set_node_probability(bn, node_enemyDefence, 3, parent_state, 0.05)//state = 11-20;

parent_state.clear();
parent_state.add(node_enemyResource, 2);
parent_state.add(node_enemyStrategy, 1);
set_node_probability(bn, node_enemyDefence, 0, parent_state, 0.1)//state = 0;
set_node_probability(bn, node_enemyDefence, 1, parent_state, 0.2)//state = 1-5;
set_node_probability(bn, node_enemyDefence, 2, parent_state, 0.3)//state = 6-10;
set_node_probability(bn, node_enemyDefence, 3, parent_state, 0.4)//state = 11-20;

parent_state.clear();
parent_state.add(node_enemyResource, 3);
parent_state.add(node_enemyStrategy, 0);
set_node_probability(bn, node_enemyDefence, 0, parent_state, 0.5)//state = 0;
set_node_probability(bn, node_enemyDefence, 1, parent_state, 0.3)//state = 1-5;
set_node_probability(bn, node_enemyDefence, 2, parent_state, 0.15)//state = 6-10;
set_node_probability(bn, node_enemyDefence, 3, parent_state, 0.05)//state = 11-20;

parent_state.clear();
parent_state.add(node_enemyResource, 3);
parent_state.add(node_enemyStrategy, 1);
set_node_probability(bn, node_enemyDefence, 0, parent_state, 0.05)//state = 0;
set_node_probability(bn, node_enemyDefence, 1, parent_state, 0.15)//state = 1-5;
set_node_probability(bn, node_enemyDefence, 2, parent_state, 0.3)//state = 6-10;
set_node_probability(bn, node_enemyDefence, 3, parent_state, 0.5)//state = 11-20;

parent_state.clear();
parent_state.add(node_enemyResource, 0);
parent_state.add(node_enemyProduction, 0);
parent_state.add(node_enemyStrategy, 0);
set_node_probability(bn, node_enemyUnits, 0, parent_state, 0.9)//state = 0;
set_node_probability(bn, node_enemyUnits, 1, parent_state, 0.05)//state = 1-10;
set_node_probability(bn, node_enemyUnits, 2, parent_state, 0.05)//state = 11-50;
set_node_probability(bn, node_enemyUnits, 3, parent_state, 0)//state = 51-300;

parent_state.clear();
parent_state.add(node_enemyResource, 0);
parent_state.add(node_enemyProduction, 0);
parent_state.add(node_enemyStrategy, 1);
set_node_probability(bn, node_enemyUnits, 0, parent_state, 0.9)//state = 0;
set_node_probability(bn, node_enemyUnits, 1, parent_state, 0.1)//state = 1-10;
set_node_probability(bn, node_enemyUnits, 2, parent_state, 0)//state = 11-50;
set_node_probability(bn, node_enemyUnits, 3, parent_state, 0)//state = 51-300;

parent_state.clear();
parent_state.add(node_enemyResource, 0);
parent_state.add(node_enemyProduction, 1);
parent_state.add(node_enemyStrategy, 0);
set_node_probability(bn, node_enemyUnits, 0, parent_state, 0.8)//state = 0;
set_node_probability(bn, node_enemyUnits, 1, parent_state, 0.15)//state = 1-10;
set_node_probability(bn, node_enemyUnits, 2, parent_state, 0.05)//state = 11-50;
set_node_probability(bn, node_enemyUnits, 3, parent_state, 0)//state = 51-300;

parent_state.clear();
parent_state.add(node_enemyResource, 0);
parent_state.add(node_enemyProduction, 1);
parent_state.add(node_enemyStrategy, 1);
set_node_probability(bn, node_enemyUnits, 0, parent_state, 0.8)//state = 0;
set_node_probability(bn, node_enemyUnits, 1, parent_state, 0.2)//state = 1-10;
set_node_probability(bn, node_enemyUnits, 2, parent_state, 0)//state = 11-50;
set_node_probability(bn, node_enemyUnits, 3, parent_state, 0)//state = 51-300;

parent_state.clear();
parent_state.add(node_enemyResource, 0);
parent_state.add(node_enemyProduction, 2);
parent_state.add(node_enemyStrategy, 0);
set_node_probability(bn, node_enemyUnits, 0, parent_state, 0.7)//state = 0;
set_node_probability(bn, node_enemyUnits, 1, parent_state, 0.2)//state = 1-10;
set_node_probability(bn, node_enemyUnits, 2, parent_state, 0.1)//state = 11-50;
set_node_probability(bn, node_enemyUnits, 3, parent_state, 0)//state = 51-300;

parent_state.clear();
parent_state.add(node_enemyResource, 0);
parent_state.add(node_enemyProduction, 2);
parent_state.add(node_enemyStrategy, 1);
set_node_probability(bn, node_enemyUnits, 0, parent_state, 0.7)//state = 0;
set_node_probability(bn, node_enemyUnits, 1, parent_state, 0.3)//state = 1-10;
set_node_probability(bn, node_enemyUnits, 2, parent_state, 0)//state = 11-50;
set_node_probability(bn, node_enemyUnits, 3, parent_state, 0)//state = 51-300;

parent_state.clear();
parent_state.add(node_enemyResource, 0);
parent_state.add(node_enemyProduction, 3);
parent_state.add(node_enemyStrategy, 0);
set_node_probability(bn, node_enemyUnits, 0, parent_state, 0.6)//state = 0;
set_node_probability(bn, node_enemyUnits, 1, parent_state, 0.3)//state = 1-10;
set_node_probability(bn, node_enemyUnits, 2, parent_state, 0.1)//state = 11-50;
set_node_probability(bn, node_enemyUnits, 3, parent_state, 0)//state = 51-300;

parent_state.clear();
parent_state.add(node_enemyResource, 0);
parent_state.add(node_enemyProduction, 3);
parent_state.add(node_enemyStrategy, 1);
set_node_probability(bn, node_enemyUnits, 0, parent_state, 0.6)//state = 0;
set_node_probability(bn, node_enemyUnits, 1, parent_state, 0.35)//state = 1-10;
set_node_probability(bn, node_enemyUnits, 2, parent_state, 0.05)//state = 11-50;
set_node_probability(bn, node_enemyUnits, 3, parent_state, 0)//state = 51-300;

parent_state.clear();
parent_state.add(node_enemyResource, 1);
parent_state.add(node_enemyProduction, 0);
parent_state.add(node_enemyStrategy, 0);
set_node_probability(bn, node_enemyUnits, 0, parent_state, 0.9)//state = 0;
set_node_probability(bn, node_enemyUnits, 1, parent_state, 0.05)//state = 1-10;
set_node_probability(bn, node_enemyUnits, 2, parent_state, 0.05)//state = 11-50;
set_node_probability(bn, node_enemyUnits, 3, parent_state, 0)//state = 51-300;

parent_state.clear();
parent_state.add(node_enemyResource, 1);
parent_state.add(node_enemyProduction, 0);
parent_state.add(node_enemyStrategy, 1);
set_node_probability(bn, node_enemyUnits, 0, parent_state, 0.8)//state = 0;
set_node_probability(bn, node_enemyUnits, 1, parent_state, 0.1)//state = 1-10;
set_node_probability(bn, node_enemyUnits, 2, parent_state, 0.1)//state = 11-50;
set_node_probability(bn, node_enemyUnits, 3, parent_state, 0)//state = 51-300;

parent_state.clear();
parent_state.add(node_enemyResource, 1);
parent_state.add(node_enemyProduction, 1);
parent_state.add(node_enemyStrategy, 0);
set_node_probability(bn, node_enemyUnits, 0, parent_state, 0.5)//state = 0;
set_node_probability(bn, node_enemyUnits, 1, parent_state, 0.2)//state = 1-10;
set_node_probability(bn, node_enemyUnits, 2, parent_state, 0.2)//state = 11-50;
set_node_probability(bn, node_enemyUnits, 3, parent_state, 0.1)//state = 51-300;

parent_state.clear();
parent_state.add(node_enemyResource, 1);
parent_state.add(node_enemyProduction, 1);
parent_state.add(node_enemyStrategy, 1);
set_node_probability(bn, node_enemyUnits, 0, parent_state, 0.3)//state = 0;
set_node_probability(bn, node_enemyUnits, 1, parent_state, 0.2)//state = 1-10;
set_node_probability(bn, node_enemyUnits, 2, parent_state, 0.3)//state = 11-50;
set_node_probability(bn, node_enemyUnits, 3, parent_state, 0.2)//state = 51-300;

parent_state.clear();
parent_state.add(node_enemyResource, 1);
parent_state.add(node_enemyProduction, 2);
parent_state.add(node_enemyStrategy, 0);
set_node_probability(bn, node_enemyUnits, 0, parent_state, 0.3)//state = 0;
set_node_probability(bn, node_enemyUnits, 1, parent_state, 0.2)//state = 1-10;
set_node_probability(bn, node_enemyUnits, 2, parent_state, 0.3)//state = 11-50;
set_node_probability(bn, node_enemyUnits, 3, parent_state, 0.2)//state = 51-300;

parent_state.clear();
parent_state.add(node_enemyResource, 1);
parent_state.add(node_enemyProduction, 2);
parent_state.add(node_enemyStrategy, 1);
set_node_probability(bn, node_enemyUnits, 0, parent_state, 0.2)//state = 0;
set_node_probability(bn, node_enemyUnits, 1, parent_state, 0.2)//state = 1-10;
set_node_probability(bn, node_enemyUnits, 2, parent_state, 0.3)//state = 11-50;
set_node_probability(bn, node_enemyUnits, 3, parent_state, 0.3)//state = 51-300;

parent_state.clear();
parent_state.add(node_enemyResource, 1);
parent_state.add(node_enemyProduction, 3);
parent_state.add(node_enemyStrategy, 0);
set_node_probability(bn, node_enemyUnits, 0, parent_state, 0.15)//state = 0;
set_node_probability(bn, node_enemyUnits, 1, parent_state, 0.25)//state = 1-10;
set_node_probability(bn, node_enemyUnits, 2, parent_state, 0.3)//state = 11-50;
set_node_probability(bn, node_enemyUnits, 3, parent_state, 0.3)//state = 51-300;

parent_state.clear();
parent_state.add(node_enemyResource, 1);
parent_state.add(node_enemyProduction, 3);
parent_state.add(node_enemyStrategy, 1);
set_node_probability(bn, node_enemyUnits, 0, parent_state, 0.1)//state = 0;
set_node_probability(bn, node_enemyUnits, 1, parent_state, 0.2)//state = 1-10;
set_node_probability(bn, node_enemyUnits, 2, parent_state, 0.3)//state = 11-50;
set_node_probability(bn, node_enemyUnits, 3, parent_state, 0.4)//state = 51-300;

parent_state.clear();
parent_state.add(node_enemyResource, 2);
parent_state.add(node_enemyProduction, 0);
parent_state.add(node_enemyStrategy, 0);
set_node_probability(bn, node_enemyUnits, 0, parent_state, 0.4)//state = 0;
set_node_probability(bn, node_enemyUnits, 1, parent_state, 0.4)//state = 1-10;
set_node_probability(bn, node_enemyUnits, 2, parent_state, 0.1)//state = 11-50;
set_node_probability(bn, node_enemyUnits, 3, parent_state, 0.1)//state = 51-300;

parent_state.clear();
parent_state.add(node_enemyResource, 2);
parent_state.add(node_enemyProduction, 0);
parent_state.add(node_enemyStrategy, 1);
set_node_probability(bn, node_enemyUnits, 0, parent_state, 0.5)//state = 0;
set_node_probability(bn, node_enemyUnits, 1, parent_state, 0.4)//state = 1-10;
set_node_probability(bn, node_enemyUnits, 2, parent_state, 0.1)//state = 11-50;
set_node_probability(bn, node_enemyUnits, 3, parent_state, 0)//state = 51-300;

parent_state.clear();
parent_state.add(node_enemyResource, 2);
parent_state.add(node_enemyProduction, 1);
parent_state.add(node_enemyStrategy, 0);
set_node_probability(bn, node_enemyUnits, 0, parent_state, 0.1)//state = 0;
set_node_probability(bn, node_enemyUnits, 1, parent_state, 0.35)//state = 1-10;
set_node_probability(bn, node_enemyUnits, 2, parent_state, 0.4)//state = 11-50;
set_node_probability(bn, node_enemyUnits, 3, parent_state, 0.15)//state = 51-300;

parent_state.clear();
parent_state.add(node_enemyResource, 2);
parent_state.add(node_enemyProduction, 1);
parent_state.add(node_enemyStrategy, 1);
set_node_probability(bn, node_enemyUnits, 0, parent_state, 0.2)//state = 0;
set_node_probability(bn, node_enemyUnits, 1, parent_state, 0.3)//state = 1-10;
set_node_probability(bn, node_enemyUnits, 2, parent_state, 0.4)//state = 11-50;
set_node_probability(bn, node_enemyUnits, 3, parent_state, 0.1)//state = 51-300;

parent_state.clear();
parent_state.add(node_enemyResource, 2);
parent_state.add(node_enemyProduction, 2);
parent_state.add(node_enemyStrategy, 0);
set_node_probability(bn, node_enemyUnits, 0, parent_state, 0.05)//state = 0;
set_node_probability(bn, node_enemyUnits, 1, parent_state, 0.15)//state = 1-10;
set_node_probability(bn, node_enemyUnits, 2, parent_state, 0.4)//state = 11-50;
set_node_probability(bn, node_enemyUnits, 3, parent_state, 0.4)//state = 51-300;

parent_state.clear();
parent_state.add(node_enemyResource, 2);
parent_state.add(node_enemyProduction, 2);
parent_state.add(node_enemyStrategy, 1);
set_node_probability(bn, node_enemyUnits, 0, parent_state, 0.1)//state = 0;
set_node_probability(bn, node_enemyUnits, 1, parent_state, 0.2)//state = 1-10;
set_node_probability(bn, node_enemyUnits, 2, parent_state, 0.3)//state = 11-50;
set_node_probability(bn, node_enemyUnits, 3, parent_state, 0.4)//state = 51-300;

parent_state.clear();
parent_state.add(node_enemyResource, 2);
parent_state.add(node_enemyProduction, 3);
parent_state.add(node_enemyStrategy, 0);
set_node_probability(bn, node_enemyUnits, 0, parent_state, 0.01)//state = 0;
set_node_probability(bn, node_enemyUnits, 1, parent_state, 0.1)//state = 1-10;
set_node_probability(bn, node_enemyUnits, 2, parent_state, 0.5)//state = 11-50;
set_node_probability(bn, node_enemyUnits, 3, parent_state, 0.39)//state = 51-300;

parent_state.clear();
parent_state.add(node_enemyResource, 2);
parent_state.add(node_enemyProduction, 3);
parent_state.add(node_enemyStrategy, 1);
set_node_probability(bn, node_enemyUnits, 0, parent_state, 0.0729927)//state = 0;
set_node_probability(bn, node_enemyUnits, 1, parent_state, 0.218978)//state = 1-10;
set_node_probability(bn, node_enemyUnits, 2, parent_state, 0.0510949)//state = 11-50;
set_node_probability(bn, node_enemyUnits, 3, parent_state, 0.656934)//state = 51-300;

parent_state.clear();
parent_state.add(node_enemyResource, 3);
parent_state.add(node_enemyProduction, 0);
parent_state.add(node_enemyStrategy, 0);
set_node_probability(bn, node_enemyUnits, 0, parent_state, 0.2)//state = 0;
set_node_probability(bn, node_enemyUnits, 1, parent_state, 0.4)//state = 1-10;
set_node_probability(bn, node_enemyUnits, 2, parent_state, 0.3)//state = 11-50;
set_node_probability(bn, node_enemyUnits, 3, parent_state, 0.1)//state = 51-300;

parent_state.clear();
parent_state.add(node_enemyResource, 3);
parent_state.add(node_enemyProduction, 0);
parent_state.add(node_enemyStrategy, 1);
set_node_probability(bn, node_enemyUnits, 0, parent_state, 0.3)//state = 0;
set_node_probability(bn, node_enemyUnits, 1, parent_state, 0.4)//state = 1-10;
set_node_probability(bn, node_enemyUnits, 2, parent_state, 0.25)//state = 11-50;
set_node_probability(bn, node_enemyUnits, 3, parent_state, 0.05)//state = 51-300;

parent_state.clear();
parent_state.add(node_enemyResource, 3);
parent_state.add(node_enemyProduction, 1);
parent_state.add(node_enemyStrategy, 0);
set_node_probability(bn, node_enemyUnits, 0, parent_state, 0.1)//state = 0;
set_node_probability(bn, node_enemyUnits, 1, parent_state, 0.2)//state = 1-10;
set_node_probability(bn, node_enemyUnits, 2, parent_state, 0.6)//state = 11-50;
set_node_probability(bn, node_enemyUnits, 3, parent_state, 0.1)//state = 51-300;

parent_state.clear();
parent_state.add(node_enemyResource, 3);
parent_state.add(node_enemyProduction, 1);
parent_state.add(node_enemyStrategy, 1);
set_node_probability(bn, node_enemyUnits, 0, parent_state, 0.2)//state = 0;
set_node_probability(bn, node_enemyUnits, 1, parent_state, 0.3)//state = 1-10;
set_node_probability(bn, node_enemyUnits, 2, parent_state, 0.4)//state = 11-50;
set_node_probability(bn, node_enemyUnits, 3, parent_state, 0.1)//state = 51-300;

parent_state.clear();
parent_state.add(node_enemyResource, 3);
parent_state.add(node_enemyProduction, 2);
parent_state.add(node_enemyStrategy, 0);
set_node_probability(bn, node_enemyUnits, 0, parent_state, 0.0454545)//state = 0;
set_node_probability(bn, node_enemyUnits, 1, parent_state, 0.136364)//state = 1-10;
set_node_probability(bn, node_enemyUnits, 2, parent_state, 0.454545)//state = 11-50;
set_node_probability(bn, node_enemyUnits, 3, parent_state, 0.363636)//state = 51-300;

parent_state.clear();
parent_state.add(node_enemyResource, 3);
parent_state.add(node_enemyProduction, 2);
parent_state.add(node_enemyStrategy, 1);
set_node_probability(bn, node_enemyUnits, 0, parent_state, 0.1)//state = 0;
set_node_probability(bn, node_enemyUnits, 1, parent_state, 0.2)//state = 1-10;
set_node_probability(bn, node_enemyUnits, 2, parent_state, 0.3)//state = 11-50;
set_node_probability(bn, node_enemyUnits, 3, parent_state, 0.4)//state = 51-300;

parent_state.clear();
parent_state.add(node_enemyResource, 3);
parent_state.add(node_enemyProduction, 3);
parent_state.add(node_enemyStrategy, 0);
set_node_probability(bn, node_enemyUnits, 0, parent_state, 0.01)//state = 0;
set_node_probability(bn, node_enemyUnits, 1, parent_state, 0.1)//state = 1-10;
set_node_probability(bn, node_enemyUnits, 2, parent_state, 0.5)//state = 11-50;
set_node_probability(bn, node_enemyUnits, 3, parent_state, 0.39)//state = 51-300;

parent_state.clear();
parent_state.add(node_enemyResource, 3);
parent_state.add(node_enemyProduction, 3);
parent_state.add(node_enemyStrategy, 1);
set_node_probability(bn, node_enemyUnits, 0, parent_state, 0.05)//state = 0;
set_node_probability(bn, node_enemyUnits, 1, parent_state, 0.15)//state = 1-10;
set_node_probability(bn, node_enemyUnits, 2, parent_state, 0.3)//state = 11-50;
set_node_probability(bn, node_enemyUnits, 3, parent_state, 0.5)//state = 51-300;

parent_state.clear();
parent_state.add(node_myStrategy, 0);
set_node_probability(bn, node_enemyStrategy, 0, parent_state, 0.5)//state = Aggressive;
set_node_probability(bn, node_enemyStrategy, 1, parent_state, 0.5)//state = Defensive;

parent_state.clear();
parent_state.add(node_myStrategy, 1);
set_node_probability(bn, node_enemyStrategy, 0, parent_state, 0.5)//state = Aggressive;
set_node_probability(bn, node_enemyStrategy, 1, parent_state, 0.5)//state = Defensive;

typedef graph::compare_1b_c, set::compare_1b_c>::kernel_1a_c join_tree_type;
join_tree_type join_tree;

create_moral_graph(bn, join_tree);
create_join_tree(join_tree, join_tree);

bayesian_network_join_tree solution(bn, join_tree);
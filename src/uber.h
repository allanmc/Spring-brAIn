bn.set_number_of_nodes(11);
bn.add_edge(node::_enemyUnits, node::_seenUnits);
bn.add_edge(node::_enemyResource, node::_seenRes);
bn.add_edge(node::_enemyProduction, node::_seenProd);
bn.add_edge(node::_enemyUnits, node::_attacks);
bn.add_edge(node::_enemyStrategy, node::_attacks);
bn.add_edge(node::_enemyDefence, node::_seenDef);
bn.add_edge(node::_enemyStrategy, node::_enemyResource);
bn.add_edge(node::_enemyResource, node::_enemyProduction);
bn.add_edge(node::_enemyStrategy, node::_enemyProduction);
bn.add_edge(node::_enemyResource, node::_enemyDefence);
bn.add_edge(node::_enemyStrategy, node::_enemyDefence);
bn.add_edge(node::_enemyResource, node::_enemyUnits);
bn.add_edge(node::_enemyProduction, node::_enemyUnits);
bn.add_edge(node::_enemyStrategy, node::_enemyUnits);
bn.add_edge(node::_myStrategy, node::_enemyStrategy);


set_node_num_values(bn, node::_seenUnits, 4);
set_node_num_values(bn, node::_myStrategy, 2);
set_node_num_values(bn, node::_seenRes, 4);
set_node_num_values(bn, node::_seenProd, 4);
set_node_num_values(bn, node::_attacks, 4);
set_node_num_values(bn, node::_seenDef, 4);
set_node_num_values(bn, node::_enemyResource, 4);
set_node_num_values(bn, node::_enemyProduction, 4);
set_node_num_values(bn, node::_enemyDefence, 4);
set_node_num_values(bn, node::_enemyUnits, 4);
set_node_num_values(bn, node::_enemyStrategy, 2);


assignment parent_state;

parent_state.clear();
parent_state.add(node::_enemyUnits, 0);
set_node_probability(bn, node::_seenUnits, 0, parent_state, 1);//state = 0
set_node_probability(bn, node::_seenUnits, 1, parent_state, 0);//state = 1-10
set_node_probability(bn, node::_seenUnits, 2, parent_state, 0);//state = 11-50
set_node_probability(bn, node::_seenUnits, 3, parent_state, 0);//state = 51-300

parent_state.clear();
parent_state.add(node::_enemyUnits, 1);
set_node_probability(bn, node::_seenUnits, 0, parent_state, 0.5);//state = 0
set_node_probability(bn, node::_seenUnits, 1, parent_state, 0.5);//state = 1-10
set_node_probability(bn, node::_seenUnits, 2, parent_state, 0);//state = 11-50
set_node_probability(bn, node::_seenUnits, 3, parent_state, 0);//state = 51-300

parent_state.clear();
parent_state.add(node::_enemyUnits, 2);
set_node_probability(bn, node::_seenUnits, 0, parent_state, 0.2);//state = 0
set_node_probability(bn, node::_seenUnits, 1, parent_state, 0.5);//state = 1-10
set_node_probability(bn, node::_seenUnits, 2, parent_state, 0.3);//state = 11-50
set_node_probability(bn, node::_seenUnits, 3, parent_state, 0);//state = 51-300

parent_state.clear();
parent_state.add(node::_enemyUnits, 3);
set_node_probability(bn, node::_seenUnits, 0, parent_state, 0.1);//state = 0
set_node_probability(bn, node::_seenUnits, 1, parent_state, 0.2);//state = 1-10
set_node_probability(bn, node::_seenUnits, 2, parent_state, 0.4);//state = 11-50
set_node_probability(bn, node::_seenUnits, 3, parent_state, 0.3);//state = 51-300

parent_state.clear();
set_node_probability(bn, node::_myStrategy, 0, parent_state, 0.5);//state = Aggressive
set_node_probability(bn, node::_myStrategy, 1, parent_state, 0.5);//state = Defensive

parent_state.clear();
parent_state.add(node::_enemyResource, 0);
set_node_probability(bn, node::_seenRes, 0, parent_state, 1);//state = 0
set_node_probability(bn, node::_seenRes, 1, parent_state, 0);//state = 1-10
set_node_probability(bn, node::_seenRes, 2, parent_state, 0);//state = 11-20
set_node_probability(bn, node::_seenRes, 3, parent_state, 0);//state = 21-40

parent_state.clear();
parent_state.add(node::_enemyResource, 1);
set_node_probability(bn, node::_seenRes, 0, parent_state, 0.5);//state = 0
set_node_probability(bn, node::_seenRes, 1, parent_state, 0.5);//state = 1-10
set_node_probability(bn, node::_seenRes, 2, parent_state, 0);//state = 11-20
set_node_probability(bn, node::_seenRes, 3, parent_state, 0);//state = 21-40

parent_state.clear();
parent_state.add(node::_enemyResource, 2);
set_node_probability(bn, node::_seenRes, 0, parent_state, 0.166667);//state = 0
set_node_probability(bn, node::_seenRes, 1, parent_state, 0.333333);//state = 1-10
set_node_probability(bn, node::_seenRes, 2, parent_state, 0.5);//state = 11-20
set_node_probability(bn, node::_seenRes, 3, parent_state, 0);//state = 21-40

parent_state.clear();
parent_state.add(node::_enemyResource, 3);
set_node_probability(bn, node::_seenRes, 0, parent_state, 0.1);//state = 0
set_node_probability(bn, node::_seenRes, 1, parent_state, 0.2);//state = 1-10
set_node_probability(bn, node::_seenRes, 2, parent_state, 0.4);//state = 11-20
set_node_probability(bn, node::_seenRes, 3, parent_state, 0.3);//state = 21-40

parent_state.clear();
parent_state.add(node::_enemyProduction, 0);
set_node_probability(bn, node::_seenProd, 0, parent_state, 1);//state = 0
set_node_probability(bn, node::_seenProd, 1, parent_state, 0);//state = 1-2
set_node_probability(bn, node::_seenProd, 2, parent_state, 0);//state = 3-6
set_node_probability(bn, node::_seenProd, 3, parent_state, 0);//state = 7-10

parent_state.clear();
parent_state.add(node::_enemyProduction, 1);
set_node_probability(bn, node::_seenProd, 0, parent_state, 0.7);//state = 0
set_node_probability(bn, node::_seenProd, 1, parent_state, 0.3);//state = 1-2
set_node_probability(bn, node::_seenProd, 2, parent_state, 0);//state = 3-6
set_node_probability(bn, node::_seenProd, 3, parent_state, 0);//state = 7-10

parent_state.clear();
parent_state.add(node::_enemyProduction, 2);
set_node_probability(bn, node::_seenProd, 0, parent_state, 0.2);//state = 0
set_node_probability(bn, node::_seenProd, 1, parent_state, 0.4);//state = 1-2
set_node_probability(bn, node::_seenProd, 2, parent_state, 0.4);//state = 3-6
set_node_probability(bn, node::_seenProd, 3, parent_state, 0);//state = 7-10

parent_state.clear();
parent_state.add(node::_enemyProduction, 3);
set_node_probability(bn, node::_seenProd, 0, parent_state, 0.1);//state = 0
set_node_probability(bn, node::_seenProd, 1, parent_state, 0.2);//state = 1-2
set_node_probability(bn, node::_seenProd, 2, parent_state, 0.4);//state = 3-6
set_node_probability(bn, node::_seenProd, 3, parent_state, 0.3);//state = 7-10

parent_state.clear();
parent_state.add(node::_enemyUnits, 0);
parent_state.add(node::_enemyStrategy, 0);
set_node_probability(bn, node::_attacks, 0, parent_state, 1);//state = 0
set_node_probability(bn, node::_attacks, 1, parent_state, 0);//state = 1-3
set_node_probability(bn, node::_attacks, 2, parent_state, 0);//state = 4-8
set_node_probability(bn, node::_attacks, 3, parent_state, 0);//state = 9-

parent_state.clear();
parent_state.add(node::_enemyUnits, 0);
parent_state.add(node::_enemyStrategy, 1);
set_node_probability(bn, node::_attacks, 0, parent_state, 1);//state = 0
set_node_probability(bn, node::_attacks, 1, parent_state, 0);//state = 1-3
set_node_probability(bn, node::_attacks, 2, parent_state, 0);//state = 4-8
set_node_probability(bn, node::_attacks, 3, parent_state, 0);//state = 9-

parent_state.clear();
parent_state.add(node::_enemyUnits, 1);
parent_state.add(node::_enemyStrategy, 0);
set_node_probability(bn, node::_attacks, 0, parent_state, 0.1);//state = 0
set_node_probability(bn, node::_attacks, 1, parent_state, 0.5);//state = 1-3
set_node_probability(bn, node::_attacks, 2, parent_state, 0.25);//state = 4-8
set_node_probability(bn, node::_attacks, 3, parent_state, 0.25);//state = 9-

parent_state.clear();
parent_state.add(node::_enemyUnits, 1);
parent_state.add(node::_enemyStrategy, 1);
set_node_probability(bn, node::_attacks, 0, parent_state, 0.5);//state = 0
set_node_probability(bn, node::_attacks, 1, parent_state, 0.3);//state = 1-3
set_node_probability(bn, node::_attacks, 2, parent_state, 0.1);//state = 4-8
set_node_probability(bn, node::_attacks, 3, parent_state, 0.1);//state = 9-

parent_state.clear();
parent_state.add(node::_enemyUnits, 2);
parent_state.add(node::_enemyStrategy, 0);
set_node_probability(bn, node::_attacks, 0, parent_state, 0.1);//state = 0
set_node_probability(bn, node::_attacks, 1, parent_state, 0.3);//state = 1-3
set_node_probability(bn, node::_attacks, 2, parent_state, 0.4);//state = 4-8
set_node_probability(bn, node::_attacks, 3, parent_state, 0.2);//state = 9-

parent_state.clear();
parent_state.add(node::_enemyUnits, 2);
parent_state.add(node::_enemyStrategy, 1);
set_node_probability(bn, node::_attacks, 0, parent_state, 0.34);//state = 0
set_node_probability(bn, node::_attacks, 1, parent_state, 0.6);//state = 1-3
set_node_probability(bn, node::_attacks, 2, parent_state, 0.05);//state = 4-8
set_node_probability(bn, node::_attacks, 3, parent_state, 0.01);//state = 9-

parent_state.clear();
parent_state.add(node::_enemyUnits, 3);
parent_state.add(node::_enemyStrategy, 0);
set_node_probability(bn, node::_attacks, 0, parent_state, 0.1);//state = 0
set_node_probability(bn, node::_attacks, 1, parent_state, 0.2);//state = 1-3
set_node_probability(bn, node::_attacks, 2, parent_state, 0.5);//state = 4-8
set_node_probability(bn, node::_attacks, 3, parent_state, 0.2);//state = 9-

parent_state.clear();
parent_state.add(node::_enemyUnits, 3);
parent_state.add(node::_enemyStrategy, 1);
set_node_probability(bn, node::_attacks, 0, parent_state, 0.2);//state = 0
set_node_probability(bn, node::_attacks, 1, parent_state, 0.65);//state = 1-3
set_node_probability(bn, node::_attacks, 2, parent_state, 0.1);//state = 4-8
set_node_probability(bn, node::_attacks, 3, parent_state, 0.05);//state = 9-

parent_state.clear();
parent_state.add(node::_enemyDefence, 0);
set_node_probability(bn, node::_seenDef, 0, parent_state, 1);//state = 0
set_node_probability(bn, node::_seenDef, 1, parent_state, 0);//state = 1-5
set_node_probability(bn, node::_seenDef, 2, parent_state, 0);//state = 6-10
set_node_probability(bn, node::_seenDef, 3, parent_state, 0);//state = 11-20

parent_state.clear();
parent_state.add(node::_enemyDefence, 1);
set_node_probability(bn, node::_seenDef, 0, parent_state, 0.6);//state = 0
set_node_probability(bn, node::_seenDef, 1, parent_state, 0.4);//state = 1-5
set_node_probability(bn, node::_seenDef, 2, parent_state, 0);//state = 6-10
set_node_probability(bn, node::_seenDef, 3, parent_state, 0);//state = 11-20

parent_state.clear();
parent_state.add(node::_enemyDefence, 2);
set_node_probability(bn, node::_seenDef, 0, parent_state, 0.4);//state = 0
set_node_probability(bn, node::_seenDef, 1, parent_state, 0.5);//state = 1-5
set_node_probability(bn, node::_seenDef, 2, parent_state, 0.1);//state = 6-10
set_node_probability(bn, node::_seenDef, 3, parent_state, 0);//state = 11-20

parent_state.clear();
parent_state.add(node::_enemyDefence, 3);
set_node_probability(bn, node::_seenDef, 0, parent_state, 0.111111);//state = 0
set_node_probability(bn, node::_seenDef, 1, parent_state, 0.111111);//state = 1-5
set_node_probability(bn, node::_seenDef, 2, parent_state, 0.222222);//state = 6-10
set_node_probability(bn, node::_seenDef, 3, parent_state, 0.555556);//state = 11-20

parent_state.clear();
parent_state.add(node::_enemyStrategy, 0);
set_node_probability(bn, node::_enemyResource, 0, parent_state, 0.05);//state = 0
set_node_probability(bn, node::_enemyResource, 1, parent_state, 0.2);//state = 1-10
set_node_probability(bn, node::_enemyResource, 2, parent_state, 0.4);//state = 11-20
set_node_probability(bn, node::_enemyResource, 3, parent_state, 0.35);//state = 21-40

parent_state.clear();
parent_state.add(node::_enemyStrategy, 1);
set_node_probability(bn, node::_enemyResource, 0, parent_state, 0.05);//state = 0
set_node_probability(bn, node::_enemyResource, 1, parent_state, 0.3);//state = 1-10
set_node_probability(bn, node::_enemyResource, 2, parent_state, 0.4);//state = 11-20
set_node_probability(bn, node::_enemyResource, 3, parent_state, 0.25);//state = 21-40

parent_state.clear();
parent_state.add(node::_enemyResource, 0);
parent_state.add(node::_enemyStrategy, 0);
set_node_probability(bn, node::_enemyProduction, 0, parent_state, 0.7);//state = 0
set_node_probability(bn, node::_enemyProduction, 1, parent_state, 0.2);//state = 1-2
set_node_probability(bn, node::_enemyProduction, 2, parent_state, 0.1);//state = 3-6
set_node_probability(bn, node::_enemyProduction, 3, parent_state, 0);//state = 7-10

parent_state.clear();
parent_state.add(node::_enemyResource, 0);
parent_state.add(node::_enemyStrategy, 1);
set_node_probability(bn, node::_enemyProduction, 0, parent_state, 0.8);//state = 0
set_node_probability(bn, node::_enemyProduction, 1, parent_state, 0.15);//state = 1-2
set_node_probability(bn, node::_enemyProduction, 2, parent_state, 0.05);//state = 3-6
set_node_probability(bn, node::_enemyProduction, 3, parent_state, 0);//state = 7-10

parent_state.clear();
parent_state.add(node::_enemyResource, 1);
parent_state.add(node::_enemyStrategy, 0);
set_node_probability(bn, node::_enemyProduction, 0, parent_state, 0.3);//state = 0
set_node_probability(bn, node::_enemyProduction, 1, parent_state, 0.5);//state = 1-2
set_node_probability(bn, node::_enemyProduction, 2, parent_state, 0.15);//state = 3-6
set_node_probability(bn, node::_enemyProduction, 3, parent_state, 0.05);//state = 7-10

parent_state.clear();
parent_state.add(node::_enemyResource, 1);
parent_state.add(node::_enemyStrategy, 1);
set_node_probability(bn, node::_enemyProduction, 0, parent_state, 0.444444);//state = 0
set_node_probability(bn, node::_enemyProduction, 1, parent_state, 0.444444);//state = 1-2
set_node_probability(bn, node::_enemyProduction, 2, parent_state, 0.0555556);//state = 3-6
set_node_probability(bn, node::_enemyProduction, 3, parent_state, 0.0555556);//state = 7-10

parent_state.clear();
parent_state.add(node::_enemyResource, 2);
parent_state.add(node::_enemyStrategy, 0);
set_node_probability(bn, node::_enemyProduction, 0, parent_state, 0.1);//state = 0
set_node_probability(bn, node::_enemyProduction, 1, parent_state, 0.5);//state = 1-2
set_node_probability(bn, node::_enemyProduction, 2, parent_state, 0.3);//state = 3-6
set_node_probability(bn, node::_enemyProduction, 3, parent_state, 0.1);//state = 7-10

parent_state.clear();
parent_state.add(node::_enemyResource, 2);
parent_state.add(node::_enemyStrategy, 1);
set_node_probability(bn, node::_enemyProduction, 0, parent_state, 0.2);//state = 0
set_node_probability(bn, node::_enemyProduction, 1, parent_state, 0.6);//state = 1-2
set_node_probability(bn, node::_enemyProduction, 2, parent_state, 0.15);//state = 3-6
set_node_probability(bn, node::_enemyProduction, 3, parent_state, 0.05);//state = 7-10

parent_state.clear();
parent_state.add(node::_enemyResource, 3);
parent_state.add(node::_enemyStrategy, 0);
set_node_probability(bn, node::_enemyProduction, 0, parent_state, 0.05);//state = 0
set_node_probability(bn, node::_enemyProduction, 1, parent_state, 0.25);//state = 1-2
set_node_probability(bn, node::_enemyProduction, 2, parent_state, 0.5);//state = 3-6
set_node_probability(bn, node::_enemyProduction, 3, parent_state, 0.2);//state = 7-10

parent_state.clear();
parent_state.add(node::_enemyResource, 3);
parent_state.add(node::_enemyStrategy, 1);
set_node_probability(bn, node::_enemyProduction, 0, parent_state, 0.1);//state = 0
set_node_probability(bn, node::_enemyProduction, 1, parent_state, 0.4);//state = 1-2
set_node_probability(bn, node::_enemyProduction, 2, parent_state, 0.45);//state = 3-6
set_node_probability(bn, node::_enemyProduction, 3, parent_state, 0.05);//state = 7-10

parent_state.clear();
parent_state.add(node::_enemyResource, 0);
parent_state.add(node::_enemyStrategy, 0);
set_node_probability(bn, node::_enemyDefence, 0, parent_state, 0.9);//state = 0
set_node_probability(bn, node::_enemyDefence, 1, parent_state, 0.04);//state = 1-5
set_node_probability(bn, node::_enemyDefence, 2, parent_state, 0.01);//state = 6-10
set_node_probability(bn, node::_enemyDefence, 3, parent_state, 0.05);//state = 11-20

parent_state.clear();
parent_state.add(node::_enemyResource, 0);
parent_state.add(node::_enemyStrategy, 1);
set_node_probability(bn, node::_enemyDefence, 0, parent_state, 0.55);//state = 0
set_node_probability(bn, node::_enemyDefence, 1, parent_state, 0.3);//state = 1-5
set_node_probability(bn, node::_enemyDefence, 2, parent_state, 0.1);//state = 6-10
set_node_probability(bn, node::_enemyDefence, 3, parent_state, 0.05);//state = 11-20

parent_state.clear();
parent_state.add(node::_enemyResource, 1);
parent_state.add(node::_enemyStrategy, 0);
set_node_probability(bn, node::_enemyDefence, 0, parent_state, 0.75);//state = 0
set_node_probability(bn, node::_enemyDefence, 1, parent_state, 0.15);//state = 1-5
set_node_probability(bn, node::_enemyDefence, 2, parent_state, 0.05);//state = 6-10
set_node_probability(bn, node::_enemyDefence, 3, parent_state, 0.05);//state = 11-20

parent_state.clear();
parent_state.add(node::_enemyResource, 1);
parent_state.add(node::_enemyStrategy, 1);
set_node_probability(bn, node::_enemyDefence, 0, parent_state, 0.1);//state = 0
set_node_probability(bn, node::_enemyDefence, 1, parent_state, 0.4);//state = 1-5
set_node_probability(bn, node::_enemyDefence, 2, parent_state, 0.3);//state = 6-10
set_node_probability(bn, node::_enemyDefence, 3, parent_state, 0.2);//state = 11-20

parent_state.clear();
parent_state.add(node::_enemyResource, 2);
parent_state.add(node::_enemyStrategy, 0);
set_node_probability(bn, node::_enemyDefence, 0, parent_state, 0.65);//state = 0
set_node_probability(bn, node::_enemyDefence, 1, parent_state, 0.2);//state = 1-5
set_node_probability(bn, node::_enemyDefence, 2, parent_state, 0.1);//state = 6-10
set_node_probability(bn, node::_enemyDefence, 3, parent_state, 0.05);//state = 11-20

parent_state.clear();
parent_state.add(node::_enemyResource, 2);
parent_state.add(node::_enemyStrategy, 1);
set_node_probability(bn, node::_enemyDefence, 0, parent_state, 0.1);//state = 0
set_node_probability(bn, node::_enemyDefence, 1, parent_state, 0.2);//state = 1-5
set_node_probability(bn, node::_enemyDefence, 2, parent_state, 0.3);//state = 6-10
set_node_probability(bn, node::_enemyDefence, 3, parent_state, 0.4);//state = 11-20

parent_state.clear();
parent_state.add(node::_enemyResource, 3);
parent_state.add(node::_enemyStrategy, 0);
set_node_probability(bn, node::_enemyDefence, 0, parent_state, 0.5);//state = 0
set_node_probability(bn, node::_enemyDefence, 1, parent_state, 0.3);//state = 1-5
set_node_probability(bn, node::_enemyDefence, 2, parent_state, 0.15);//state = 6-10
set_node_probability(bn, node::_enemyDefence, 3, parent_state, 0.05);//state = 11-20

parent_state.clear();
parent_state.add(node::_enemyResource, 3);
parent_state.add(node::_enemyStrategy, 1);
set_node_probability(bn, node::_enemyDefence, 0, parent_state, 0.05);//state = 0
set_node_probability(bn, node::_enemyDefence, 1, parent_state, 0.15);//state = 1-5
set_node_probability(bn, node::_enemyDefence, 2, parent_state, 0.3);//state = 6-10
set_node_probability(bn, node::_enemyDefence, 3, parent_state, 0.5);//state = 11-20

parent_state.clear();
parent_state.add(node::_enemyResource, 0);
parent_state.add(node::_enemyProduction, 0);
parent_state.add(node::_enemyStrategy, 0);
set_node_probability(bn, node::_enemyUnits, 0, parent_state, 0.9);//state = 0
set_node_probability(bn, node::_enemyUnits, 1, parent_state, 0.05);//state = 1-10
set_node_probability(bn, node::_enemyUnits, 2, parent_state, 0.05);//state = 11-50
set_node_probability(bn, node::_enemyUnits, 3, parent_state, 0);//state = 51-300

parent_state.clear();
parent_state.add(node::_enemyResource, 0);
parent_state.add(node::_enemyProduction, 0);
parent_state.add(node::_enemyStrategy, 1);
set_node_probability(bn, node::_enemyUnits, 0, parent_state, 0.9);//state = 0
set_node_probability(bn, node::_enemyUnits, 1, parent_state, 0.1);//state = 1-10
set_node_probability(bn, node::_enemyUnits, 2, parent_state, 0);//state = 11-50
set_node_probability(bn, node::_enemyUnits, 3, parent_state, 0);//state = 51-300

parent_state.clear();
parent_state.add(node::_enemyResource, 0);
parent_state.add(node::_enemyProduction, 1);
parent_state.add(node::_enemyStrategy, 0);
set_node_probability(bn, node::_enemyUnits, 0, parent_state, 0.8);//state = 0
set_node_probability(bn, node::_enemyUnits, 1, parent_state, 0.15);//state = 1-10
set_node_probability(bn, node::_enemyUnits, 2, parent_state, 0.05);//state = 11-50
set_node_probability(bn, node::_enemyUnits, 3, parent_state, 0);//state = 51-300

parent_state.clear();
parent_state.add(node::_enemyResource, 0);
parent_state.add(node::_enemyProduction, 1);
parent_state.add(node::_enemyStrategy, 1);
set_node_probability(bn, node::_enemyUnits, 0, parent_state, 0.8);//state = 0
set_node_probability(bn, node::_enemyUnits, 1, parent_state, 0.2);//state = 1-10
set_node_probability(bn, node::_enemyUnits, 2, parent_state, 0);//state = 11-50
set_node_probability(bn, node::_enemyUnits, 3, parent_state, 0);//state = 51-300

parent_state.clear();
parent_state.add(node::_enemyResource, 0);
parent_state.add(node::_enemyProduction, 2);
parent_state.add(node::_enemyStrategy, 0);
set_node_probability(bn, node::_enemyUnits, 0, parent_state, 0.7);//state = 0
set_node_probability(bn, node::_enemyUnits, 1, parent_state, 0.2);//state = 1-10
set_node_probability(bn, node::_enemyUnits, 2, parent_state, 0.1);//state = 11-50
set_node_probability(bn, node::_enemyUnits, 3, parent_state, 0);//state = 51-300

parent_state.clear();
parent_state.add(node::_enemyResource, 0);
parent_state.add(node::_enemyProduction, 2);
parent_state.add(node::_enemyStrategy, 1);
set_node_probability(bn, node::_enemyUnits, 0, parent_state, 0.7);//state = 0
set_node_probability(bn, node::_enemyUnits, 1, parent_state, 0.3);//state = 1-10
set_node_probability(bn, node::_enemyUnits, 2, parent_state, 0);//state = 11-50
set_node_probability(bn, node::_enemyUnits, 3, parent_state, 0);//state = 51-300

parent_state.clear();
parent_state.add(node::_enemyResource, 0);
parent_state.add(node::_enemyProduction, 3);
parent_state.add(node::_enemyStrategy, 0);
set_node_probability(bn, node::_enemyUnits, 0, parent_state, 0.6);//state = 0
set_node_probability(bn, node::_enemyUnits, 1, parent_state, 0.3);//state = 1-10
set_node_probability(bn, node::_enemyUnits, 2, parent_state, 0.1);//state = 11-50
set_node_probability(bn, node::_enemyUnits, 3, parent_state, 0);//state = 51-300

parent_state.clear();
parent_state.add(node::_enemyResource, 0);
parent_state.add(node::_enemyProduction, 3);
parent_state.add(node::_enemyStrategy, 1);
set_node_probability(bn, node::_enemyUnits, 0, parent_state, 0.6);//state = 0
set_node_probability(bn, node::_enemyUnits, 1, parent_state, 0.35);//state = 1-10
set_node_probability(bn, node::_enemyUnits, 2, parent_state, 0.05);//state = 11-50
set_node_probability(bn, node::_enemyUnits, 3, parent_state, 0);//state = 51-300

parent_state.clear();
parent_state.add(node::_enemyResource, 1);
parent_state.add(node::_enemyProduction, 0);
parent_state.add(node::_enemyStrategy, 0);
set_node_probability(bn, node::_enemyUnits, 0, parent_state, 0.9);//state = 0
set_node_probability(bn, node::_enemyUnits, 1, parent_state, 0.05);//state = 1-10
set_node_probability(bn, node::_enemyUnits, 2, parent_state, 0.05);//state = 11-50
set_node_probability(bn, node::_enemyUnits, 3, parent_state, 0);//state = 51-300

parent_state.clear();
parent_state.add(node::_enemyResource, 1);
parent_state.add(node::_enemyProduction, 0);
parent_state.add(node::_enemyStrategy, 1);
set_node_probability(bn, node::_enemyUnits, 0, parent_state, 0.8);//state = 0
set_node_probability(bn, node::_enemyUnits, 1, parent_state, 0.1);//state = 1-10
set_node_probability(bn, node::_enemyUnits, 2, parent_state, 0.1);//state = 11-50
set_node_probability(bn, node::_enemyUnits, 3, parent_state, 0);//state = 51-300

parent_state.clear();
parent_state.add(node::_enemyResource, 1);
parent_state.add(node::_enemyProduction, 1);
parent_state.add(node::_enemyStrategy, 0);
set_node_probability(bn, node::_enemyUnits, 0, parent_state, 0.5);//state = 0
set_node_probability(bn, node::_enemyUnits, 1, parent_state, 0.2);//state = 1-10
set_node_probability(bn, node::_enemyUnits, 2, parent_state, 0.2);//state = 11-50
set_node_probability(bn, node::_enemyUnits, 3, parent_state, 0.1);//state = 51-300

parent_state.clear();
parent_state.add(node::_enemyResource, 1);
parent_state.add(node::_enemyProduction, 1);
parent_state.add(node::_enemyStrategy, 1);
set_node_probability(bn, node::_enemyUnits, 0, parent_state, 0.3);//state = 0
set_node_probability(bn, node::_enemyUnits, 1, parent_state, 0.2);//state = 1-10
set_node_probability(bn, node::_enemyUnits, 2, parent_state, 0.3);//state = 11-50
set_node_probability(bn, node::_enemyUnits, 3, parent_state, 0.2);//state = 51-300

parent_state.clear();
parent_state.add(node::_enemyResource, 1);
parent_state.add(node::_enemyProduction, 2);
parent_state.add(node::_enemyStrategy, 0);
set_node_probability(bn, node::_enemyUnits, 0, parent_state, 0.3);//state = 0
set_node_probability(bn, node::_enemyUnits, 1, parent_state, 0.2);//state = 1-10
set_node_probability(bn, node::_enemyUnits, 2, parent_state, 0.3);//state = 11-50
set_node_probability(bn, node::_enemyUnits, 3, parent_state, 0.2);//state = 51-300

parent_state.clear();
parent_state.add(node::_enemyResource, 1);
parent_state.add(node::_enemyProduction, 2);
parent_state.add(node::_enemyStrategy, 1);
set_node_probability(bn, node::_enemyUnits, 0, parent_state, 0.2);//state = 0
set_node_probability(bn, node::_enemyUnits, 1, parent_state, 0.2);//state = 1-10
set_node_probability(bn, node::_enemyUnits, 2, parent_state, 0.3);//state = 11-50
set_node_probability(bn, node::_enemyUnits, 3, parent_state, 0.3);//state = 51-300

parent_state.clear();
parent_state.add(node::_enemyResource, 1);
parent_state.add(node::_enemyProduction, 3);
parent_state.add(node::_enemyStrategy, 0);
set_node_probability(bn, node::_enemyUnits, 0, parent_state, 0.15);//state = 0
set_node_probability(bn, node::_enemyUnits, 1, parent_state, 0.25);//state = 1-10
set_node_probability(bn, node::_enemyUnits, 2, parent_state, 0.3);//state = 11-50
set_node_probability(bn, node::_enemyUnits, 3, parent_state, 0.3);//state = 51-300

parent_state.clear();
parent_state.add(node::_enemyResource, 1);
parent_state.add(node::_enemyProduction, 3);
parent_state.add(node::_enemyStrategy, 1);
set_node_probability(bn, node::_enemyUnits, 0, parent_state, 0.1);//state = 0
set_node_probability(bn, node::_enemyUnits, 1, parent_state, 0.2);//state = 1-10
set_node_probability(bn, node::_enemyUnits, 2, parent_state, 0.3);//state = 11-50
set_node_probability(bn, node::_enemyUnits, 3, parent_state, 0.4);//state = 51-300

parent_state.clear();
parent_state.add(node::_enemyResource, 2);
parent_state.add(node::_enemyProduction, 0);
parent_state.add(node::_enemyStrategy, 0);
set_node_probability(bn, node::_enemyUnits, 0, parent_state, 0.4);//state = 0
set_node_probability(bn, node::_enemyUnits, 1, parent_state, 0.4);//state = 1-10
set_node_probability(bn, node::_enemyUnits, 2, parent_state, 0.1);//state = 11-50
set_node_probability(bn, node::_enemyUnits, 3, parent_state, 0.1);//state = 51-300

parent_state.clear();
parent_state.add(node::_enemyResource, 2);
parent_state.add(node::_enemyProduction, 0);
parent_state.add(node::_enemyStrategy, 1);
set_node_probability(bn, node::_enemyUnits, 0, parent_state, 0.5);//state = 0
set_node_probability(bn, node::_enemyUnits, 1, parent_state, 0.4);//state = 1-10
set_node_probability(bn, node::_enemyUnits, 2, parent_state, 0.1);//state = 11-50
set_node_probability(bn, node::_enemyUnits, 3, parent_state, 0);//state = 51-300

parent_state.clear();
parent_state.add(node::_enemyResource, 2);
parent_state.add(node::_enemyProduction, 1);
parent_state.add(node::_enemyStrategy, 0);
set_node_probability(bn, node::_enemyUnits, 0, parent_state, 0.1);//state = 0
set_node_probability(bn, node::_enemyUnits, 1, parent_state, 0.35);//state = 1-10
set_node_probability(bn, node::_enemyUnits, 2, parent_state, 0.4);//state = 11-50
set_node_probability(bn, node::_enemyUnits, 3, parent_state, 0.15);//state = 51-300

parent_state.clear();
parent_state.add(node::_enemyResource, 2);
parent_state.add(node::_enemyProduction, 1);
parent_state.add(node::_enemyStrategy, 1);
set_node_probability(bn, node::_enemyUnits, 0, parent_state, 0.2);//state = 0
set_node_probability(bn, node::_enemyUnits, 1, parent_state, 0.3);//state = 1-10
set_node_probability(bn, node::_enemyUnits, 2, parent_state, 0.4);//state = 11-50
set_node_probability(bn, node::_enemyUnits, 3, parent_state, 0.1);//state = 51-300

parent_state.clear();
parent_state.add(node::_enemyResource, 2);
parent_state.add(node::_enemyProduction, 2);
parent_state.add(node::_enemyStrategy, 0);
set_node_probability(bn, node::_enemyUnits, 0, parent_state, 0.05);//state = 0
set_node_probability(bn, node::_enemyUnits, 1, parent_state, 0.15);//state = 1-10
set_node_probability(bn, node::_enemyUnits, 2, parent_state, 0.4);//state = 11-50
set_node_probability(bn, node::_enemyUnits, 3, parent_state, 0.4);//state = 51-300

parent_state.clear();
parent_state.add(node::_enemyResource, 2);
parent_state.add(node::_enemyProduction, 2);
parent_state.add(node::_enemyStrategy, 1);
set_node_probability(bn, node::_enemyUnits, 0, parent_state, 0.1);//state = 0
set_node_probability(bn, node::_enemyUnits, 1, parent_state, 0.2);//state = 1-10
set_node_probability(bn, node::_enemyUnits, 2, parent_state, 0.3);//state = 11-50
set_node_probability(bn, node::_enemyUnits, 3, parent_state, 0.4);//state = 51-300

parent_state.clear();
parent_state.add(node::_enemyResource, 2);
parent_state.add(node::_enemyProduction, 3);
parent_state.add(node::_enemyStrategy, 0);
set_node_probability(bn, node::_enemyUnits, 0, parent_state, 0.01);//state = 0
set_node_probability(bn, node::_enemyUnits, 1, parent_state, 0.1);//state = 1-10
set_node_probability(bn, node::_enemyUnits, 2, parent_state, 0.5);//state = 11-50
set_node_probability(bn, node::_enemyUnits, 3, parent_state, 0.39);//state = 51-300

parent_state.clear();
parent_state.add(node::_enemyResource, 2);
parent_state.add(node::_enemyProduction, 3);
parent_state.add(node::_enemyStrategy, 1);
set_node_probability(bn, node::_enemyUnits, 0, parent_state, 0.0729927);//state = 0
set_node_probability(bn, node::_enemyUnits, 1, parent_state, 0.218978);//state = 1-10
set_node_probability(bn, node::_enemyUnits, 2, parent_state, 0.0510949);//state = 11-50
set_node_probability(bn, node::_enemyUnits, 3, parent_state, 0.656934);//state = 51-300

parent_state.clear();
parent_state.add(node::_enemyResource, 3);
parent_state.add(node::_enemyProduction, 0);
parent_state.add(node::_enemyStrategy, 0);
set_node_probability(bn, node::_enemyUnits, 0, parent_state, 0.2);//state = 0
set_node_probability(bn, node::_enemyUnits, 1, parent_state, 0.4);//state = 1-10
set_node_probability(bn, node::_enemyUnits, 2, parent_state, 0.3);//state = 11-50
set_node_probability(bn, node::_enemyUnits, 3, parent_state, 0.1);//state = 51-300

parent_state.clear();
parent_state.add(node::_enemyResource, 3);
parent_state.add(node::_enemyProduction, 0);
parent_state.add(node::_enemyStrategy, 1);
set_node_probability(bn, node::_enemyUnits, 0, parent_state, 0.3);//state = 0
set_node_probability(bn, node::_enemyUnits, 1, parent_state, 0.4);//state = 1-10
set_node_probability(bn, node::_enemyUnits, 2, parent_state, 0.25);//state = 11-50
set_node_probability(bn, node::_enemyUnits, 3, parent_state, 0.05);//state = 51-300

parent_state.clear();
parent_state.add(node::_enemyResource, 3);
parent_state.add(node::_enemyProduction, 1);
parent_state.add(node::_enemyStrategy, 0);
set_node_probability(bn, node::_enemyUnits, 0, parent_state, 0.1);//state = 0
set_node_probability(bn, node::_enemyUnits, 1, parent_state, 0.2);//state = 1-10
set_node_probability(bn, node::_enemyUnits, 2, parent_state, 0.6);//state = 11-50
set_node_probability(bn, node::_enemyUnits, 3, parent_state, 0.1);//state = 51-300

parent_state.clear();
parent_state.add(node::_enemyResource, 3);
parent_state.add(node::_enemyProduction, 1);
parent_state.add(node::_enemyStrategy, 1);
set_node_probability(bn, node::_enemyUnits, 0, parent_state, 0.2);//state = 0
set_node_probability(bn, node::_enemyUnits, 1, parent_state, 0.3);//state = 1-10
set_node_probability(bn, node::_enemyUnits, 2, parent_state, 0.4);//state = 11-50
set_node_probability(bn, node::_enemyUnits, 3, parent_state, 0.1);//state = 51-300

parent_state.clear();
parent_state.add(node::_enemyResource, 3);
parent_state.add(node::_enemyProduction, 2);
parent_state.add(node::_enemyStrategy, 0);
set_node_probability(bn, node::_enemyUnits, 0, parent_state, 0.0454545);//state = 0
set_node_probability(bn, node::_enemyUnits, 1, parent_state, 0.136364);//state = 1-10
set_node_probability(bn, node::_enemyUnits, 2, parent_state, 0.454545);//state = 11-50
set_node_probability(bn, node::_enemyUnits, 3, parent_state, 0.363636);//state = 51-300

parent_state.clear();
parent_state.add(node::_enemyResource, 3);
parent_state.add(node::_enemyProduction, 2);
parent_state.add(node::_enemyStrategy, 1);
set_node_probability(bn, node::_enemyUnits, 0, parent_state, 0.1);//state = 0
set_node_probability(bn, node::_enemyUnits, 1, parent_state, 0.2);//state = 1-10
set_node_probability(bn, node::_enemyUnits, 2, parent_state, 0.3);//state = 11-50
set_node_probability(bn, node::_enemyUnits, 3, parent_state, 0.4);//state = 51-300

parent_state.clear();
parent_state.add(node::_enemyResource, 3);
parent_state.add(node::_enemyProduction, 3);
parent_state.add(node::_enemyStrategy, 0);
set_node_probability(bn, node::_enemyUnits, 0, parent_state, 0.01);//state = 0
set_node_probability(bn, node::_enemyUnits, 1, parent_state, 0.1);//state = 1-10
set_node_probability(bn, node::_enemyUnits, 2, parent_state, 0.5);//state = 11-50
set_node_probability(bn, node::_enemyUnits, 3, parent_state, 0.39);//state = 51-300

parent_state.clear();
parent_state.add(node::_enemyResource, 3);
parent_state.add(node::_enemyProduction, 3);
parent_state.add(node::_enemyStrategy, 1);
set_node_probability(bn, node::_enemyUnits, 0, parent_state, 0.05);//state = 0
set_node_probability(bn, node::_enemyUnits, 1, parent_state, 0.15);//state = 1-10
set_node_probability(bn, node::_enemyUnits, 2, parent_state, 0.3);//state = 11-50
set_node_probability(bn, node::_enemyUnits, 3, parent_state, 0.5);//state = 51-300

parent_state.clear();
parent_state.add(node::_myStrategy, 0);
set_node_probability(bn, node::_enemyStrategy, 0, parent_state, 0.5);//state = Aggressive
set_node_probability(bn, node::_enemyStrategy, 1, parent_state, 0.5);//state = Defensive

parent_state.clear();
parent_state.add(node::_myStrategy, 1);
set_node_probability(bn, node::_enemyStrategy, 0, parent_state, 0.5);//state = Aggressive
set_node_probability(bn, node::_enemyStrategy, 1, parent_state, 0.5);//state = Defensive


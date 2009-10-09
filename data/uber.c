bn.set_number_of_nodes(11);
bn.add_edge(node::enemyUnits::id, node::seenUnits::id);
bn.add_edge(node::enemyResource::id, node::seenRes::id);
bn.add_edge(node::enemyProduction::id, node::seenProd::id);
bn.add_edge(node::enemyUnits::id, node::attacks::id);
bn.add_edge(node::enemyStrategy::id, node::attacks::id);
bn.add_edge(node::enemyDefence::id, node::seenDef::id);
bn.add_edge(node::enemyStrategy::id, node::enemyResource::id);
bn.add_edge(node::enemyResource::id, node::enemyProduction::id);
bn.add_edge(node::enemyStrategy::id, node::enemyProduction::id);
bn.add_edge(node::enemyResource::id, node::enemyDefence::id);
bn.add_edge(node::enemyStrategy::id, node::enemyDefence::id);
bn.add_edge(node::enemyResource::id, node::enemyUnits::id);
bn.add_edge(node::enemyProduction::id, node::enemyUnits::id);
bn.add_edge(node::enemyStrategy::id, node::enemyUnits::id);
bn.add_edge(node::myStrategy::id, node::enemyStrategy::id);


set_node_num_values(bn, node::seenUnits::id, 4);
set_node_num_values(bn, node::myStrategy::id, 2);
set_node_num_values(bn, node::seenRes::id, 4);
set_node_num_values(bn, node::seenProd::id, 4);
set_node_num_values(bn, node::attacks::id, 4);
set_node_num_values(bn, node::seenDef::id, 4);
set_node_num_values(bn, node::enemyResource::id, 4);
set_node_num_values(bn, node::enemyProduction::id, 4);
set_node_num_values(bn, node::enemyDefence::id, 4);
set_node_num_values(bn, node::enemyUnits::id, 4);
set_node_num_values(bn, node::enemyStrategy::id, 2);


assignment parent_state;

parent_state.clear();
parent_state.add(node::enemyUnits::id, 0);
set_node_probability(bn, node::seenUnits::id, 0, parent_state, 1);//state = 0
set_node_probability(bn, node::seenUnits::id, 1, parent_state, 0);//state = 1-10
set_node_probability(bn, node::seenUnits::id, 2, parent_state, 0);//state = 11-50
set_node_probability(bn, node::seenUnits::id, 3, parent_state, 0);//state = 51-300

parent_state.clear();
parent_state.add(node::enemyUnits::id, 1);
set_node_probability(bn, node::seenUnits::id, 0, parent_state, 0.5);//state = 0
set_node_probability(bn, node::seenUnits::id, 1, parent_state, 0.5);//state = 1-10
set_node_probability(bn, node::seenUnits::id, 2, parent_state, 0);//state = 11-50
set_node_probability(bn, node::seenUnits::id, 3, parent_state, 0);//state = 51-300

parent_state.clear();
parent_state.add(node::enemyUnits::id, 2);
set_node_probability(bn, node::seenUnits::id, 0, parent_state, 0.2);//state = 0
set_node_probability(bn, node::seenUnits::id, 1, parent_state, 0.5);//state = 1-10
set_node_probability(bn, node::seenUnits::id, 2, parent_state, 0.3);//state = 11-50
set_node_probability(bn, node::seenUnits::id, 3, parent_state, 0);//state = 51-300

parent_state.clear();
parent_state.add(node::enemyUnits::id, 3);
set_node_probability(bn, node::seenUnits::id, 0, parent_state, 0.1);//state = 0
set_node_probability(bn, node::seenUnits::id, 1, parent_state, 0.2);//state = 1-10
set_node_probability(bn, node::seenUnits::id, 2, parent_state, 0.4);//state = 11-50
set_node_probability(bn, node::seenUnits::id, 3, parent_state, 0.3);//state = 51-300

parent_state.clear();
set_node_probability(bn, node::myStrategy::id, 0, parent_state, 0.5);//state = Aggressive
set_node_probability(bn, node::myStrategy::id, 1, parent_state, 0.5);//state = Defensive

parent_state.clear();
parent_state.add(node::enemyResource::id, 0);
set_node_probability(bn, node::seenRes::id, 0, parent_state, 1);//state = 0
set_node_probability(bn, node::seenRes::id, 1, parent_state, 0);//state = 1-10
set_node_probability(bn, node::seenRes::id, 2, parent_state, 0);//state = 11-20
set_node_probability(bn, node::seenRes::id, 3, parent_state, 0);//state = 21-40

parent_state.clear();
parent_state.add(node::enemyResource::id, 1);
set_node_probability(bn, node::seenRes::id, 0, parent_state, 0.5);//state = 0
set_node_probability(bn, node::seenRes::id, 1, parent_state, 0.5);//state = 1-10
set_node_probability(bn, node::seenRes::id, 2, parent_state, 0);//state = 11-20
set_node_probability(bn, node::seenRes::id, 3, parent_state, 0);//state = 21-40

parent_state.clear();
parent_state.add(node::enemyResource::id, 2);
set_node_probability(bn, node::seenRes::id, 0, parent_state, 0.166667);//state = 0
set_node_probability(bn, node::seenRes::id, 1, parent_state, 0.333333);//state = 1-10
set_node_probability(bn, node::seenRes::id, 2, parent_state, 0.5);//state = 11-20
set_node_probability(bn, node::seenRes::id, 3, parent_state, 0);//state = 21-40

parent_state.clear();
parent_state.add(node::enemyResource::id, 3);
set_node_probability(bn, node::seenRes::id, 0, parent_state, 0.1);//state = 0
set_node_probability(bn, node::seenRes::id, 1, parent_state, 0.2);//state = 1-10
set_node_probability(bn, node::seenRes::id, 2, parent_state, 0.4);//state = 11-20
set_node_probability(bn, node::seenRes::id, 3, parent_state, 0.3);//state = 21-40

parent_state.clear();
parent_state.add(node::enemyProduction::id, 0);
set_node_probability(bn, node::seenProd::id, 0, parent_state, 1);//state = 0
set_node_probability(bn, node::seenProd::id, 1, parent_state, 0);//state = 1-2
set_node_probability(bn, node::seenProd::id, 2, parent_state, 0);//state = 3-6
set_node_probability(bn, node::seenProd::id, 3, parent_state, 0);//state = 7-10

parent_state.clear();
parent_state.add(node::enemyProduction::id, 1);
set_node_probability(bn, node::seenProd::id, 0, parent_state, 0.7);//state = 0
set_node_probability(bn, node::seenProd::id, 1, parent_state, 0.3);//state = 1-2
set_node_probability(bn, node::seenProd::id, 2, parent_state, 0);//state = 3-6
set_node_probability(bn, node::seenProd::id, 3, parent_state, 0);//state = 7-10

parent_state.clear();
parent_state.add(node::enemyProduction::id, 2);
set_node_probability(bn, node::seenProd::id, 0, parent_state, 0.2);//state = 0
set_node_probability(bn, node::seenProd::id, 1, parent_state, 0.4);//state = 1-2
set_node_probability(bn, node::seenProd::id, 2, parent_state, 0.4);//state = 3-6
set_node_probability(bn, node::seenProd::id, 3, parent_state, 0);//state = 7-10

parent_state.clear();
parent_state.add(node::enemyProduction::id, 3);
set_node_probability(bn, node::seenProd::id, 0, parent_state, 0.1);//state = 0
set_node_probability(bn, node::seenProd::id, 1, parent_state, 0.2);//state = 1-2
set_node_probability(bn, node::seenProd::id, 2, parent_state, 0.4);//state = 3-6
set_node_probability(bn, node::seenProd::id, 3, parent_state, 0.3);//state = 7-10

parent_state.clear();
parent_state.add(node::enemyUnits::id, 0);
parent_state.add(node::enemyStrategy::id, 0);
set_node_probability(bn, node::attacks::id, 0, parent_state, 1);//state = 0
set_node_probability(bn, node::attacks::id, 1, parent_state, 0);//state = 1-3
set_node_probability(bn, node::attacks::id, 2, parent_state, 0);//state = 4-8
set_node_probability(bn, node::attacks::id, 3, parent_state, 0);//state = 9-

parent_state.clear();
parent_state.add(node::enemyUnits::id, 0);
parent_state.add(node::enemyStrategy::id, 1);
set_node_probability(bn, node::attacks::id, 0, parent_state, 1);//state = 0
set_node_probability(bn, node::attacks::id, 1, parent_state, 0);//state = 1-3
set_node_probability(bn, node::attacks::id, 2, parent_state, 0);//state = 4-8
set_node_probability(bn, node::attacks::id, 3, parent_state, 0);//state = 9-

parent_state.clear();
parent_state.add(node::enemyUnits::id, 1);
parent_state.add(node::enemyStrategy::id, 0);
set_node_probability(bn, node::attacks::id, 0, parent_state, 0.1);//state = 0
set_node_probability(bn, node::attacks::id, 1, parent_state, 0.5);//state = 1-3
set_node_probability(bn, node::attacks::id, 2, parent_state, 0.25);//state = 4-8
set_node_probability(bn, node::attacks::id, 3, parent_state, 0.25);//state = 9-

parent_state.clear();
parent_state.add(node::enemyUnits::id, 1);
parent_state.add(node::enemyStrategy::id, 1);
set_node_probability(bn, node::attacks::id, 0, parent_state, 0.5);//state = 0
set_node_probability(bn, node::attacks::id, 1, parent_state, 0.3);//state = 1-3
set_node_probability(bn, node::attacks::id, 2, parent_state, 0.1);//state = 4-8
set_node_probability(bn, node::attacks::id, 3, parent_state, 0.1);//state = 9-

parent_state.clear();
parent_state.add(node::enemyUnits::id, 2);
parent_state.add(node::enemyStrategy::id, 0);
set_node_probability(bn, node::attacks::id, 0, parent_state, 0.1);//state = 0
set_node_probability(bn, node::attacks::id, 1, parent_state, 0.3);//state = 1-3
set_node_probability(bn, node::attacks::id, 2, parent_state, 0.4);//state = 4-8
set_node_probability(bn, node::attacks::id, 3, parent_state, 0.2);//state = 9-

parent_state.clear();
parent_state.add(node::enemyUnits::id, 2);
parent_state.add(node::enemyStrategy::id, 1);
set_node_probability(bn, node::attacks::id, 0, parent_state, 0.34);//state = 0
set_node_probability(bn, node::attacks::id, 1, parent_state, 0.6);//state = 1-3
set_node_probability(bn, node::attacks::id, 2, parent_state, 0.05);//state = 4-8
set_node_probability(bn, node::attacks::id, 3, parent_state, 0.01);//state = 9-

parent_state.clear();
parent_state.add(node::enemyUnits::id, 3);
parent_state.add(node::enemyStrategy::id, 0);
set_node_probability(bn, node::attacks::id, 0, parent_state, 0.1);//state = 0
set_node_probability(bn, node::attacks::id, 1, parent_state, 0.2);//state = 1-3
set_node_probability(bn, node::attacks::id, 2, parent_state, 0.5);//state = 4-8
set_node_probability(bn, node::attacks::id, 3, parent_state, 0.2);//state = 9-

parent_state.clear();
parent_state.add(node::enemyUnits::id, 3);
parent_state.add(node::enemyStrategy::id, 1);
set_node_probability(bn, node::attacks::id, 0, parent_state, 0.2);//state = 0
set_node_probability(bn, node::attacks::id, 1, parent_state, 0.65);//state = 1-3
set_node_probability(bn, node::attacks::id, 2, parent_state, 0.1);//state = 4-8
set_node_probability(bn, node::attacks::id, 3, parent_state, 0.05);//state = 9-

parent_state.clear();
parent_state.add(node::enemyDefence::id, 0);
set_node_probability(bn, node::seenDef::id, 0, parent_state, 1);//state = 0
set_node_probability(bn, node::seenDef::id, 1, parent_state, 0);//state = 1-5
set_node_probability(bn, node::seenDef::id, 2, parent_state, 0);//state = 6-10
set_node_probability(bn, node::seenDef::id, 3, parent_state, 0);//state = 11-20

parent_state.clear();
parent_state.add(node::enemyDefence::id, 1);
set_node_probability(bn, node::seenDef::id, 0, parent_state, 0.6);//state = 0
set_node_probability(bn, node::seenDef::id, 1, parent_state, 0.4);//state = 1-5
set_node_probability(bn, node::seenDef::id, 2, parent_state, 0);//state = 6-10
set_node_probability(bn, node::seenDef::id, 3, parent_state, 0);//state = 11-20

parent_state.clear();
parent_state.add(node::enemyDefence::id, 2);
set_node_probability(bn, node::seenDef::id, 0, parent_state, 0.4);//state = 0
set_node_probability(bn, node::seenDef::id, 1, parent_state, 0.5);//state = 1-5
set_node_probability(bn, node::seenDef::id, 2, parent_state, 0.1);//state = 6-10
set_node_probability(bn, node::seenDef::id, 3, parent_state, 0);//state = 11-20

parent_state.clear();
parent_state.add(node::enemyDefence::id, 3);
set_node_probability(bn, node::seenDef::id, 0, parent_state, 0.111111);//state = 0
set_node_probability(bn, node::seenDef::id, 1, parent_state, 0.111111);//state = 1-5
set_node_probability(bn, node::seenDef::id, 2, parent_state, 0.222222);//state = 6-10
set_node_probability(bn, node::seenDef::id, 3, parent_state, 0.555556);//state = 11-20

parent_state.clear();
parent_state.add(node::enemyStrategy::id, 0);
set_node_probability(bn, node::enemyResource::id, 0, parent_state, 0.05);//state = 0
set_node_probability(bn, node::enemyResource::id, 1, parent_state, 0.2);//state = 1-10
set_node_probability(bn, node::enemyResource::id, 2, parent_state, 0.4);//state = 11-20
set_node_probability(bn, node::enemyResource::id, 3, parent_state, 0.35);//state = 21-40

parent_state.clear();
parent_state.add(node::enemyStrategy::id, 1);
set_node_probability(bn, node::enemyResource::id, 0, parent_state, 0.05);//state = 0
set_node_probability(bn, node::enemyResource::id, 1, parent_state, 0.3);//state = 1-10
set_node_probability(bn, node::enemyResource::id, 2, parent_state, 0.4);//state = 11-20
set_node_probability(bn, node::enemyResource::id, 3, parent_state, 0.25);//state = 21-40

parent_state.clear();
parent_state.add(node::enemyResource::id, 0);
parent_state.add(node::enemyStrategy::id, 0);
set_node_probability(bn, node::enemyProduction::id, 0, parent_state, 0.7);//state = 0
set_node_probability(bn, node::enemyProduction::id, 1, parent_state, 0.2);//state = 1-2
set_node_probability(bn, node::enemyProduction::id, 2, parent_state, 0.1);//state = 3-6
set_node_probability(bn, node::enemyProduction::id, 3, parent_state, 0);//state = 7-10

parent_state.clear();
parent_state.add(node::enemyResource::id, 0);
parent_state.add(node::enemyStrategy::id, 1);
set_node_probability(bn, node::enemyProduction::id, 0, parent_state, 0.8);//state = 0
set_node_probability(bn, node::enemyProduction::id, 1, parent_state, 0.15);//state = 1-2
set_node_probability(bn, node::enemyProduction::id, 2, parent_state, 0.05);//state = 3-6
set_node_probability(bn, node::enemyProduction::id, 3, parent_state, 0);//state = 7-10

parent_state.clear();
parent_state.add(node::enemyResource::id, 1);
parent_state.add(node::enemyStrategy::id, 0);
set_node_probability(bn, node::enemyProduction::id, 0, parent_state, 0.3);//state = 0
set_node_probability(bn, node::enemyProduction::id, 1, parent_state, 0.5);//state = 1-2
set_node_probability(bn, node::enemyProduction::id, 2, parent_state, 0.15);//state = 3-6
set_node_probability(bn, node::enemyProduction::id, 3, parent_state, 0.05);//state = 7-10

parent_state.clear();
parent_state.add(node::enemyResource::id, 1);
parent_state.add(node::enemyStrategy::id, 1);
set_node_probability(bn, node::enemyProduction::id, 0, parent_state, 0.444444);//state = 0
set_node_probability(bn, node::enemyProduction::id, 1, parent_state, 0.444444);//state = 1-2
set_node_probability(bn, node::enemyProduction::id, 2, parent_state, 0.0555556);//state = 3-6
set_node_probability(bn, node::enemyProduction::id, 3, parent_state, 0.0555556);//state = 7-10

parent_state.clear();
parent_state.add(node::enemyResource::id, 2);
parent_state.add(node::enemyStrategy::id, 0);
set_node_probability(bn, node::enemyProduction::id, 0, parent_state, 0.1);//state = 0
set_node_probability(bn, node::enemyProduction::id, 1, parent_state, 0.5);//state = 1-2
set_node_probability(bn, node::enemyProduction::id, 2, parent_state, 0.3);//state = 3-6
set_node_probability(bn, node::enemyProduction::id, 3, parent_state, 0.1);//state = 7-10

parent_state.clear();
parent_state.add(node::enemyResource::id, 2);
parent_state.add(node::enemyStrategy::id, 1);
set_node_probability(bn, node::enemyProduction::id, 0, parent_state, 0.2);//state = 0
set_node_probability(bn, node::enemyProduction::id, 1, parent_state, 0.6);//state = 1-2
set_node_probability(bn, node::enemyProduction::id, 2, parent_state, 0.15);//state = 3-6
set_node_probability(bn, node::enemyProduction::id, 3, parent_state, 0.05);//state = 7-10

parent_state.clear();
parent_state.add(node::enemyResource::id, 3);
parent_state.add(node::enemyStrategy::id, 0);
set_node_probability(bn, node::enemyProduction::id, 0, parent_state, 0.05);//state = 0
set_node_probability(bn, node::enemyProduction::id, 1, parent_state, 0.25);//state = 1-2
set_node_probability(bn, node::enemyProduction::id, 2, parent_state, 0.5);//state = 3-6
set_node_probability(bn, node::enemyProduction::id, 3, parent_state, 0.2);//state = 7-10

parent_state.clear();
parent_state.add(node::enemyResource::id, 3);
parent_state.add(node::enemyStrategy::id, 1);
set_node_probability(bn, node::enemyProduction::id, 0, parent_state, 0.1);//state = 0
set_node_probability(bn, node::enemyProduction::id, 1, parent_state, 0.4);//state = 1-2
set_node_probability(bn, node::enemyProduction::id, 2, parent_state, 0.45);//state = 3-6
set_node_probability(bn, node::enemyProduction::id, 3, parent_state, 0.05);//state = 7-10

parent_state.clear();
parent_state.add(node::enemyResource::id, 0);
parent_state.add(node::enemyStrategy::id, 0);
set_node_probability(bn, node::enemyDefence::id, 0, parent_state, 0.9);//state = 0
set_node_probability(bn, node::enemyDefence::id, 1, parent_state, 0.04);//state = 1-5
set_node_probability(bn, node::enemyDefence::id, 2, parent_state, 0.01);//state = 6-10
set_node_probability(bn, node::enemyDefence::id, 3, parent_state, 0.05);//state = 11-20

parent_state.clear();
parent_state.add(node::enemyResource::id, 0);
parent_state.add(node::enemyStrategy::id, 1);
set_node_probability(bn, node::enemyDefence::id, 0, parent_state, 0.55);//state = 0
set_node_probability(bn, node::enemyDefence::id, 1, parent_state, 0.3);//state = 1-5
set_node_probability(bn, node::enemyDefence::id, 2, parent_state, 0.1);//state = 6-10
set_node_probability(bn, node::enemyDefence::id, 3, parent_state, 0.05);//state = 11-20

parent_state.clear();
parent_state.add(node::enemyResource::id, 1);
parent_state.add(node::enemyStrategy::id, 0);
set_node_probability(bn, node::enemyDefence::id, 0, parent_state, 0.75);//state = 0
set_node_probability(bn, node::enemyDefence::id, 1, parent_state, 0.15);//state = 1-5
set_node_probability(bn, node::enemyDefence::id, 2, parent_state, 0.05);//state = 6-10
set_node_probability(bn, node::enemyDefence::id, 3, parent_state, 0.05);//state = 11-20

parent_state.clear();
parent_state.add(node::enemyResource::id, 1);
parent_state.add(node::enemyStrategy::id, 1);
set_node_probability(bn, node::enemyDefence::id, 0, parent_state, 0.1);//state = 0
set_node_probability(bn, node::enemyDefence::id, 1, parent_state, 0.4);//state = 1-5
set_node_probability(bn, node::enemyDefence::id, 2, parent_state, 0.3);//state = 6-10
set_node_probability(bn, node::enemyDefence::id, 3, parent_state, 0.2);//state = 11-20

parent_state.clear();
parent_state.add(node::enemyResource::id, 2);
parent_state.add(node::enemyStrategy::id, 0);
set_node_probability(bn, node::enemyDefence::id, 0, parent_state, 0.65);//state = 0
set_node_probability(bn, node::enemyDefence::id, 1, parent_state, 0.2);//state = 1-5
set_node_probability(bn, node::enemyDefence::id, 2, parent_state, 0.1);//state = 6-10
set_node_probability(bn, node::enemyDefence::id, 3, parent_state, 0.05);//state = 11-20

parent_state.clear();
parent_state.add(node::enemyResource::id, 2);
parent_state.add(node::enemyStrategy::id, 1);
set_node_probability(bn, node::enemyDefence::id, 0, parent_state, 0.1);//state = 0
set_node_probability(bn, node::enemyDefence::id, 1, parent_state, 0.2);//state = 1-5
set_node_probability(bn, node::enemyDefence::id, 2, parent_state, 0.3);//state = 6-10
set_node_probability(bn, node::enemyDefence::id, 3, parent_state, 0.4);//state = 11-20

parent_state.clear();
parent_state.add(node::enemyResource::id, 3);
parent_state.add(node::enemyStrategy::id, 0);
set_node_probability(bn, node::enemyDefence::id, 0, parent_state, 0.5);//state = 0
set_node_probability(bn, node::enemyDefence::id, 1, parent_state, 0.3);//state = 1-5
set_node_probability(bn, node::enemyDefence::id, 2, parent_state, 0.15);//state = 6-10
set_node_probability(bn, node::enemyDefence::id, 3, parent_state, 0.05);//state = 11-20

parent_state.clear();
parent_state.add(node::enemyResource::id, 3);
parent_state.add(node::enemyStrategy::id, 1);
set_node_probability(bn, node::enemyDefence::id, 0, parent_state, 0.05);//state = 0
set_node_probability(bn, node::enemyDefence::id, 1, parent_state, 0.15);//state = 1-5
set_node_probability(bn, node::enemyDefence::id, 2, parent_state, 0.3);//state = 6-10
set_node_probability(bn, node::enemyDefence::id, 3, parent_state, 0.5);//state = 11-20

parent_state.clear();
parent_state.add(node::enemyResource::id, 0);
parent_state.add(node::enemyProduction::id, 0);
parent_state.add(node::enemyStrategy::id, 0);
set_node_probability(bn, node::enemyUnits::id, 0, parent_state, 0.9);//state = 0
set_node_probability(bn, node::enemyUnits::id, 1, parent_state, 0.05);//state = 1-10
set_node_probability(bn, node::enemyUnits::id, 2, parent_state, 0.05);//state = 11-50
set_node_probability(bn, node::enemyUnits::id, 3, parent_state, 0);//state = 51-300

parent_state.clear();
parent_state.add(node::enemyResource::id, 0);
parent_state.add(node::enemyProduction::id, 0);
parent_state.add(node::enemyStrategy::id, 1);
set_node_probability(bn, node::enemyUnits::id, 0, parent_state, 0.9);//state = 0
set_node_probability(bn, node::enemyUnits::id, 1, parent_state, 0.1);//state = 1-10
set_node_probability(bn, node::enemyUnits::id, 2, parent_state, 0);//state = 11-50
set_node_probability(bn, node::enemyUnits::id, 3, parent_state, 0);//state = 51-300

parent_state.clear();
parent_state.add(node::enemyResource::id, 0);
parent_state.add(node::enemyProduction::id, 1);
parent_state.add(node::enemyStrategy::id, 0);
set_node_probability(bn, node::enemyUnits::id, 0, parent_state, 0.8);//state = 0
set_node_probability(bn, node::enemyUnits::id, 1, parent_state, 0.15);//state = 1-10
set_node_probability(bn, node::enemyUnits::id, 2, parent_state, 0.05);//state = 11-50
set_node_probability(bn, node::enemyUnits::id, 3, parent_state, 0);//state = 51-300

parent_state.clear();
parent_state.add(node::enemyResource::id, 0);
parent_state.add(node::enemyProduction::id, 1);
parent_state.add(node::enemyStrategy::id, 1);
set_node_probability(bn, node::enemyUnits::id, 0, parent_state, 0.8);//state = 0
set_node_probability(bn, node::enemyUnits::id, 1, parent_state, 0.2);//state = 1-10
set_node_probability(bn, node::enemyUnits::id, 2, parent_state, 0);//state = 11-50
set_node_probability(bn, node::enemyUnits::id, 3, parent_state, 0);//state = 51-300

parent_state.clear();
parent_state.add(node::enemyResource::id, 0);
parent_state.add(node::enemyProduction::id, 2);
parent_state.add(node::enemyStrategy::id, 0);
set_node_probability(bn, node::enemyUnits::id, 0, parent_state, 0.7);//state = 0
set_node_probability(bn, node::enemyUnits::id, 1, parent_state, 0.2);//state = 1-10
set_node_probability(bn, node::enemyUnits::id, 2, parent_state, 0.1);//state = 11-50
set_node_probability(bn, node::enemyUnits::id, 3, parent_state, 0);//state = 51-300

parent_state.clear();
parent_state.add(node::enemyResource::id, 0);
parent_state.add(node::enemyProduction::id, 2);
parent_state.add(node::enemyStrategy::id, 1);
set_node_probability(bn, node::enemyUnits::id, 0, parent_state, 0.7);//state = 0
set_node_probability(bn, node::enemyUnits::id, 1, parent_state, 0.3);//state = 1-10
set_node_probability(bn, node::enemyUnits::id, 2, parent_state, 0);//state = 11-50
set_node_probability(bn, node::enemyUnits::id, 3, parent_state, 0);//state = 51-300

parent_state.clear();
parent_state.add(node::enemyResource::id, 0);
parent_state.add(node::enemyProduction::id, 3);
parent_state.add(node::enemyStrategy::id, 0);
set_node_probability(bn, node::enemyUnits::id, 0, parent_state, 0.6);//state = 0
set_node_probability(bn, node::enemyUnits::id, 1, parent_state, 0.3);//state = 1-10
set_node_probability(bn, node::enemyUnits::id, 2, parent_state, 0.1);//state = 11-50
set_node_probability(bn, node::enemyUnits::id, 3, parent_state, 0);//state = 51-300

parent_state.clear();
parent_state.add(node::enemyResource::id, 0);
parent_state.add(node::enemyProduction::id, 3);
parent_state.add(node::enemyStrategy::id, 1);
set_node_probability(bn, node::enemyUnits::id, 0, parent_state, 0.6);//state = 0
set_node_probability(bn, node::enemyUnits::id, 1, parent_state, 0.35);//state = 1-10
set_node_probability(bn, node::enemyUnits::id, 2, parent_state, 0.05);//state = 11-50
set_node_probability(bn, node::enemyUnits::id, 3, parent_state, 0);//state = 51-300

parent_state.clear();
parent_state.add(node::enemyResource::id, 1);
parent_state.add(node::enemyProduction::id, 0);
parent_state.add(node::enemyStrategy::id, 0);
set_node_probability(bn, node::enemyUnits::id, 0, parent_state, 0.9);//state = 0
set_node_probability(bn, node::enemyUnits::id, 1, parent_state, 0.05);//state = 1-10
set_node_probability(bn, node::enemyUnits::id, 2, parent_state, 0.05);//state = 11-50
set_node_probability(bn, node::enemyUnits::id, 3, parent_state, 0);//state = 51-300

parent_state.clear();
parent_state.add(node::enemyResource::id, 1);
parent_state.add(node::enemyProduction::id, 0);
parent_state.add(node::enemyStrategy::id, 1);
set_node_probability(bn, node::enemyUnits::id, 0, parent_state, 0.8);//state = 0
set_node_probability(bn, node::enemyUnits::id, 1, parent_state, 0.1);//state = 1-10
set_node_probability(bn, node::enemyUnits::id, 2, parent_state, 0.1);//state = 11-50
set_node_probability(bn, node::enemyUnits::id, 3, parent_state, 0);//state = 51-300

parent_state.clear();
parent_state.add(node::enemyResource::id, 1);
parent_state.add(node::enemyProduction::id, 1);
parent_state.add(node::enemyStrategy::id, 0);
set_node_probability(bn, node::enemyUnits::id, 0, parent_state, 0.5);//state = 0
set_node_probability(bn, node::enemyUnits::id, 1, parent_state, 0.2);//state = 1-10
set_node_probability(bn, node::enemyUnits::id, 2, parent_state, 0.2);//state = 11-50
set_node_probability(bn, node::enemyUnits::id, 3, parent_state, 0.1);//state = 51-300

parent_state.clear();
parent_state.add(node::enemyResource::id, 1);
parent_state.add(node::enemyProduction::id, 1);
parent_state.add(node::enemyStrategy::id, 1);
set_node_probability(bn, node::enemyUnits::id, 0, parent_state, 0.3);//state = 0
set_node_probability(bn, node::enemyUnits::id, 1, parent_state, 0.2);//state = 1-10
set_node_probability(bn, node::enemyUnits::id, 2, parent_state, 0.3);//state = 11-50
set_node_probability(bn, node::enemyUnits::id, 3, parent_state, 0.2);//state = 51-300

parent_state.clear();
parent_state.add(node::enemyResource::id, 1);
parent_state.add(node::enemyProduction::id, 2);
parent_state.add(node::enemyStrategy::id, 0);
set_node_probability(bn, node::enemyUnits::id, 0, parent_state, 0.3);//state = 0
set_node_probability(bn, node::enemyUnits::id, 1, parent_state, 0.2);//state = 1-10
set_node_probability(bn, node::enemyUnits::id, 2, parent_state, 0.3);//state = 11-50
set_node_probability(bn, node::enemyUnits::id, 3, parent_state, 0.2);//state = 51-300

parent_state.clear();
parent_state.add(node::enemyResource::id, 1);
parent_state.add(node::enemyProduction::id, 2);
parent_state.add(node::enemyStrategy::id, 1);
set_node_probability(bn, node::enemyUnits::id, 0, parent_state, 0.2);//state = 0
set_node_probability(bn, node::enemyUnits::id, 1, parent_state, 0.2);//state = 1-10
set_node_probability(bn, node::enemyUnits::id, 2, parent_state, 0.3);//state = 11-50
set_node_probability(bn, node::enemyUnits::id, 3, parent_state, 0.3);//state = 51-300

parent_state.clear();
parent_state.add(node::enemyResource::id, 1);
parent_state.add(node::enemyProduction::id, 3);
parent_state.add(node::enemyStrategy::id, 0);
set_node_probability(bn, node::enemyUnits::id, 0, parent_state, 0.15);//state = 0
set_node_probability(bn, node::enemyUnits::id, 1, parent_state, 0.25);//state = 1-10
set_node_probability(bn, node::enemyUnits::id, 2, parent_state, 0.3);//state = 11-50
set_node_probability(bn, node::enemyUnits::id, 3, parent_state, 0.3);//state = 51-300

parent_state.clear();
parent_state.add(node::enemyResource::id, 1);
parent_state.add(node::enemyProduction::id, 3);
parent_state.add(node::enemyStrategy::id, 1);
set_node_probability(bn, node::enemyUnits::id, 0, parent_state, 0.1);//state = 0
set_node_probability(bn, node::enemyUnits::id, 1, parent_state, 0.2);//state = 1-10
set_node_probability(bn, node::enemyUnits::id, 2, parent_state, 0.3);//state = 11-50
set_node_probability(bn, node::enemyUnits::id, 3, parent_state, 0.4);//state = 51-300

parent_state.clear();
parent_state.add(node::enemyResource::id, 2);
parent_state.add(node::enemyProduction::id, 0);
parent_state.add(node::enemyStrategy::id, 0);
set_node_probability(bn, node::enemyUnits::id, 0, parent_state, 0.4);//state = 0
set_node_probability(bn, node::enemyUnits::id, 1, parent_state, 0.4);//state = 1-10
set_node_probability(bn, node::enemyUnits::id, 2, parent_state, 0.1);//state = 11-50
set_node_probability(bn, node::enemyUnits::id, 3, parent_state, 0.1);//state = 51-300

parent_state.clear();
parent_state.add(node::enemyResource::id, 2);
parent_state.add(node::enemyProduction::id, 0);
parent_state.add(node::enemyStrategy::id, 1);
set_node_probability(bn, node::enemyUnits::id, 0, parent_state, 0.5);//state = 0
set_node_probability(bn, node::enemyUnits::id, 1, parent_state, 0.4);//state = 1-10
set_node_probability(bn, node::enemyUnits::id, 2, parent_state, 0.1);//state = 11-50
set_node_probability(bn, node::enemyUnits::id, 3, parent_state, 0);//state = 51-300

parent_state.clear();
parent_state.add(node::enemyResource::id, 2);
parent_state.add(node::enemyProduction::id, 1);
parent_state.add(node::enemyStrategy::id, 0);
set_node_probability(bn, node::enemyUnits::id, 0, parent_state, 0.1);//state = 0
set_node_probability(bn, node::enemyUnits::id, 1, parent_state, 0.35);//state = 1-10
set_node_probability(bn, node::enemyUnits::id, 2, parent_state, 0.4);//state = 11-50
set_node_probability(bn, node::enemyUnits::id, 3, parent_state, 0.15);//state = 51-300

parent_state.clear();
parent_state.add(node::enemyResource::id, 2);
parent_state.add(node::enemyProduction::id, 1);
parent_state.add(node::enemyStrategy::id, 1);
set_node_probability(bn, node::enemyUnits::id, 0, parent_state, 0.2);//state = 0
set_node_probability(bn, node::enemyUnits::id, 1, parent_state, 0.3);//state = 1-10
set_node_probability(bn, node::enemyUnits::id, 2, parent_state, 0.4);//state = 11-50
set_node_probability(bn, node::enemyUnits::id, 3, parent_state, 0.1);//state = 51-300

parent_state.clear();
parent_state.add(node::enemyResource::id, 2);
parent_state.add(node::enemyProduction::id, 2);
parent_state.add(node::enemyStrategy::id, 0);
set_node_probability(bn, node::enemyUnits::id, 0, parent_state, 0.05);//state = 0
set_node_probability(bn, node::enemyUnits::id, 1, parent_state, 0.15);//state = 1-10
set_node_probability(bn, node::enemyUnits::id, 2, parent_state, 0.4);//state = 11-50
set_node_probability(bn, node::enemyUnits::id, 3, parent_state, 0.4);//state = 51-300

parent_state.clear();
parent_state.add(node::enemyResource::id, 2);
parent_state.add(node::enemyProduction::id, 2);
parent_state.add(node::enemyStrategy::id, 1);
set_node_probability(bn, node::enemyUnits::id, 0, parent_state, 0.1);//state = 0
set_node_probability(bn, node::enemyUnits::id, 1, parent_state, 0.2);//state = 1-10
set_node_probability(bn, node::enemyUnits::id, 2, parent_state, 0.3);//state = 11-50
set_node_probability(bn, node::enemyUnits::id, 3, parent_state, 0.4);//state = 51-300

parent_state.clear();
parent_state.add(node::enemyResource::id, 2);
parent_state.add(node::enemyProduction::id, 3);
parent_state.add(node::enemyStrategy::id, 0);
set_node_probability(bn, node::enemyUnits::id, 0, parent_state, 0.01);//state = 0
set_node_probability(bn, node::enemyUnits::id, 1, parent_state, 0.1);//state = 1-10
set_node_probability(bn, node::enemyUnits::id, 2, parent_state, 0.5);//state = 11-50
set_node_probability(bn, node::enemyUnits::id, 3, parent_state, 0.39);//state = 51-300

parent_state.clear();
parent_state.add(node::enemyResource::id, 2);
parent_state.add(node::enemyProduction::id, 3);
parent_state.add(node::enemyStrategy::id, 1);
set_node_probability(bn, node::enemyUnits::id, 0, parent_state, 0.0729927);//state = 0
set_node_probability(bn, node::enemyUnits::id, 1, parent_state, 0.218978);//state = 1-10
set_node_probability(bn, node::enemyUnits::id, 2, parent_state, 0.0510949);//state = 11-50
set_node_probability(bn, node::enemyUnits::id, 3, parent_state, 0.656934);//state = 51-300

parent_state.clear();
parent_state.add(node::enemyResource::id, 3);
parent_state.add(node::enemyProduction::id, 0);
parent_state.add(node::enemyStrategy::id, 0);
set_node_probability(bn, node::enemyUnits::id, 0, parent_state, 0.2);//state = 0
set_node_probability(bn, node::enemyUnits::id, 1, parent_state, 0.4);//state = 1-10
set_node_probability(bn, node::enemyUnits::id, 2, parent_state, 0.3);//state = 11-50
set_node_probability(bn, node::enemyUnits::id, 3, parent_state, 0.1);//state = 51-300

parent_state.clear();
parent_state.add(node::enemyResource::id, 3);
parent_state.add(node::enemyProduction::id, 0);
parent_state.add(node::enemyStrategy::id, 1);
set_node_probability(bn, node::enemyUnits::id, 0, parent_state, 0.3);//state = 0
set_node_probability(bn, node::enemyUnits::id, 1, parent_state, 0.4);//state = 1-10
set_node_probability(bn, node::enemyUnits::id, 2, parent_state, 0.25);//state = 11-50
set_node_probability(bn, node::enemyUnits::id, 3, parent_state, 0.05);//state = 51-300

parent_state.clear();
parent_state.add(node::enemyResource::id, 3);
parent_state.add(node::enemyProduction::id, 1);
parent_state.add(node::enemyStrategy::id, 0);
set_node_probability(bn, node::enemyUnits::id, 0, parent_state, 0.1);//state = 0
set_node_probability(bn, node::enemyUnits::id, 1, parent_state, 0.2);//state = 1-10
set_node_probability(bn, node::enemyUnits::id, 2, parent_state, 0.6);//state = 11-50
set_node_probability(bn, node::enemyUnits::id, 3, parent_state, 0.1);//state = 51-300

parent_state.clear();
parent_state.add(node::enemyResource::id, 3);
parent_state.add(node::enemyProduction::id, 1);
parent_state.add(node::enemyStrategy::id, 1);
set_node_probability(bn, node::enemyUnits::id, 0, parent_state, 0.2);//state = 0
set_node_probability(bn, node::enemyUnits::id, 1, parent_state, 0.3);//state = 1-10
set_node_probability(bn, node::enemyUnits::id, 2, parent_state, 0.4);//state = 11-50
set_node_probability(bn, node::enemyUnits::id, 3, parent_state, 0.1);//state = 51-300

parent_state.clear();
parent_state.add(node::enemyResource::id, 3);
parent_state.add(node::enemyProduction::id, 2);
parent_state.add(node::enemyStrategy::id, 0);
set_node_probability(bn, node::enemyUnits::id, 0, parent_state, 0.0454545);//state = 0
set_node_probability(bn, node::enemyUnits::id, 1, parent_state, 0.136364);//state = 1-10
set_node_probability(bn, node::enemyUnits::id, 2, parent_state, 0.454545);//state = 11-50
set_node_probability(bn, node::enemyUnits::id, 3, parent_state, 0.363636);//state = 51-300

parent_state.clear();
parent_state.add(node::enemyResource::id, 3);
parent_state.add(node::enemyProduction::id, 2);
parent_state.add(node::enemyStrategy::id, 1);
set_node_probability(bn, node::enemyUnits::id, 0, parent_state, 0.1);//state = 0
set_node_probability(bn, node::enemyUnits::id, 1, parent_state, 0.2);//state = 1-10
set_node_probability(bn, node::enemyUnits::id, 2, parent_state, 0.3);//state = 11-50
set_node_probability(bn, node::enemyUnits::id, 3, parent_state, 0.4);//state = 51-300

parent_state.clear();
parent_state.add(node::enemyResource::id, 3);
parent_state.add(node::enemyProduction::id, 3);
parent_state.add(node::enemyStrategy::id, 0);
set_node_probability(bn, node::enemyUnits::id, 0, parent_state, 0.01);//state = 0
set_node_probability(bn, node::enemyUnits::id, 1, parent_state, 0.1);//state = 1-10
set_node_probability(bn, node::enemyUnits::id, 2, parent_state, 0.5);//state = 11-50
set_node_probability(bn, node::enemyUnits::id, 3, parent_state, 0.39);//state = 51-300

parent_state.clear();
parent_state.add(node::enemyResource::id, 3);
parent_state.add(node::enemyProduction::id, 3);
parent_state.add(node::enemyStrategy::id, 1);
set_node_probability(bn, node::enemyUnits::id, 0, parent_state, 0.05);//state = 0
set_node_probability(bn, node::enemyUnits::id, 1, parent_state, 0.15);//state = 1-10
set_node_probability(bn, node::enemyUnits::id, 2, parent_state, 0.3);//state = 11-50
set_node_probability(bn, node::enemyUnits::id, 3, parent_state, 0.5);//state = 51-300

parent_state.clear();
parent_state.add(node::myStrategy::id, 0);
set_node_probability(bn, node::enemyStrategy::id, 0, parent_state, 0.5);//state = Aggressive
set_node_probability(bn, node::enemyStrategy::id, 1, parent_state, 0.5);//state = Defensive

parent_state.clear();
parent_state.add(node::myStrategy::id, 1);
set_node_probability(bn, node::enemyStrategy::id, 0, parent_state, 0.5);//state = Aggressive
set_node_probability(bn, node::enemyStrategy::id, 1, parent_state, 0.5);//state = Defensive


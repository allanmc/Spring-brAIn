<form action="index.php" method="post" enctype="multipart/form-data">
	<input type="file" name="file" id="file" />
	<input type="submit" name="submit" value="Submit" />
</form>

<?php

if ($_FILES["file"]["error"] > 0) {
	die("Filupload fejlede!");
} else {
	$data = file_get_contents($_FILES["file"]["tmp_name"]);
}

//Nodes and decisions
$nodes_outer = "/(node|decision)[\s]*([^\s]*)[\s]*{[\s]*((([^\s]*) = ([^;]+);[\s]*)*)}/i";
$nodes_inner = "/([^\s]*) = [\"\(]?([^;]*?)[\"\)]?;[\s]*/i";
$state_labels = "/\"([^\"]*)\"/i";

$potential_outer = "/potential[\s]*\(([^\s]+)( \| ([^\)]+))?\)[\s]*{[\s]*data[\s]*=([\s]*[^}]+)}/i";
$potential_inner = "/[\(]+([^\)]+)[\)]+/i";

$nodes = new Nodes();;

//Parse nodes & decisions
preg_match_all($nodes_outer, $data, $matches, PREG_SET_ORDER);

foreach ($matches as $match) {
	$node = new Node($match[1], $match[2]);
	$nodes->addNode($node);
	preg_match_all($nodes_inner, $match[3], $settings, PREG_SET_ORDER);
	foreach ($settings as $setting) {
		if ($setting[1] == 'states') {
			preg_match_all($state_labels, $setting[2], $states, PREG_SET_ORDER);
			foreach ($states as $state) {
				$node->addState($state[1]);
			}
			break;
		}
	}
}

//Parse potentials
preg_match_all($potential_outer, $data, $matches, PREG_SET_ORDER);

foreach ($matches as $match) {
	$node = $nodes->findNode($match[1]);
	$node_counter = array();
	if ($match[3]) {//Any influences?
		$influences = explode(' ', $match[3]);
		
		foreach($influences as $influence) {
			$i = $node->addInfluence($influence);
			$node_counter[] = array("curState"=>0, "numStates"=>$nodes->findNode($influence)->numStates());
		}
	}
	preg_match_all($potential_inner, $match[4], $potentials, PREG_SET_ORDER);
	
	
	
	foreach ($potentials as $potential) {
		$values = explode(' ', trim($potential[1]));
		$givenPotential = new GivenPotential();
		$givenPotential->values = $values;
		for ($i = 0; $i < count($node_counter); $i++) {
			$givenPotential->dependencies[$i] = $node_counter[$i]['curState'];
		}
		
		for($i = count($node_counter)-1; $i >= 0; $i--) {
			$node_counter[$i]['curState']++;
			if ($node_counter[$i]['curState']>=$node_counter[$i]['numStates']) {
				$node_counter[$i]['curState'] = 0;
			} else {
				break;
			}
		}
		$node->addPotentials($givenPotential);
	}
}

echo '<pre>';
//echo htmlentities("directed_graph<bayes_node>::kernel_1a_c bn;\n\n");
echo htmlentities($nodes);
$return = <<<END
typedef graph<set<unsigned long>::compare_1b_c, set<unsigned long>::compare_1b_c>::kernel_1a_c join_tree_type;
join_tree_type join_tree;

create_moral_graph(bn, join_tree);
create_join_tree(join_tree, join_tree);

bayesian_network_join_tree solution(bn, join_tree);
END;
//echo htmlentities($return);

echo '</pre>';
//print_r($nodes);

class GivenPotential{
	public $values = array();
	public $dependencies = array();
}
class Node {
	public $name = '';
	public $type = '';
	public $states = array();
	public $influences = array();
	public $potentials = array();
	function __construct($type, $name) {
		$this->type = $type;
		$this->name = $name;
	}
	
	public function getInfluenceById($id) {
		if ($id < 0 || $id >= count($this->influences)) {
			return;
		}
		return $this->influences[$id];
	}
	
	public function addPotentials($potentials) {
		$this->potentials[] = $potentials;
	}
	
	public function addState($name) {
		$this->states[] = $name;
	}
	
	public function addInfluence(&$node) {
		$this->influences[] = $node;
		return count($this->influences)-1;
	}
	
	public function numStates() {
		return count($this->states);
	}
}

class Nodes {
	private $nodes = array();
	
	public function addNode(&$node) {
		$this->nodes[] = $node;
	}
	
	public function findNode($name) {
		foreach ($this->nodes as &$node) {
			if ($node->name == $name) {
				return $node;
				break;
			}
		}
		return null;
	}
	
    public function __toString()
    {
		$return = "";
		
		$bn = "bn";
		$node_namespace = "node";
		$prefix = "$node_namespace::_";
		$id_name = "id";
		$suffix = "";
		$parent = "parent_state";
		
		$return .= "namespace $node_namespace\n{\n";
		$return .= "\tenum node_ids\n\t{\n";
        for ($i = 0; $i < count($this->nodes); $i++) {
			$return .= "\t\t_".$this->nodes[$i]->name.' = '.$i;
			if ($i!=count($this->nodes)-1) $return .= ",";
			$return .= "\n";
		}
		$return .= "\t};\n";
		
        for ($i = 0; $i < count($this->nodes); $i++) {
			$return .= "\tnamespace ".$this->nodes[$i]->name."\n\t{\n";
			//$return .= "\t\tint $id_name = $i;\n";
			$return .= "\t\tenum State\n\t\t{\n";
			for ($j = 0; $j < count($this->nodes[$i]->states); $j++) {
				$return .= "\t\t\t_".str_replace("-", "_", $this->nodes[$i]->states[$j]).' = '.$j;
				if ($j!=count($this->nodes[$i]->states)-1) $return .= ",";
				$return .= "\n";
			}
			$return .= "\t\t};\n\t}\n";
		}
		$return .= "}\n\n";
		
		

		
		$return .= $bn.".set_number_of_nodes(".count($this->nodes).");\n";
        foreach ($this->nodes as $node) {
			foreach ($node->influences as $influence) {
				$return .= $bn.".add_edge(".$prefix.$influence.$suffix.", ".$prefix.$node->name.$suffix.");\n";
			}
		}
		$return .= "\n\n";
        foreach ($this->nodes as $node) {
			$return .= "set_node_num_values(".$bn.", ".$prefix.$node->name.$suffix.", ".$node->numStates().");\n";
		}
		$return .= "\n\nassignment ".$parent.";\n\n";
		
        foreach ($this->nodes as $node) {
			foreach ($node->potentials as $potential) {
				$return .= $parent.".clear();\n";
				if (count($potential->dependencies)>0) {
					foreach ($potential->dependencies as $dependency=>$state) {
						$return .= $parent.".add(".$prefix.$node->getInfluenceById($dependency).$suffix.", ".$state.");\n";
					}
				}
				for ($i = 0; $i < $node->numStates(); $i++) {
					
					$return .= "set_node_probability(".$bn.", ".$prefix.$node->name.$suffix.", ".$i.", ".$parent.", ".$potential->values[$i].");//state = ".$node->states[$i]."\n";
				}
				$return .= "\n";
			}
		}
		
		return $return;
    }
}

?>
<?php

if ($argc!=2)
	die("What do you want from me!?!?"); 

$filename = $argv[1].'{N}.txt';



$averages = array();
$header = "";
$i = 1;

while (($currentfile=str_replace('{N}', $i, $filename)) && file_exists($currentfile)) {
	//echo "I am parsing: $currentfile\n";
	$data = file_get_contents($currentfile);
	$data = explode("\n", $data);
	if ($header=="") $header = $data[0];
	for ($x = 1; $x < count($data); $x++) {
		if ($data[$x]==="") continue;
		$averages[$x] = ($averages[$x]/$i)*($i-1)+($data[$x]/$i);
	}
	$i++;
}

echo $header . "\r\n";
foreach ($averages as $average) {
	echo $average . "\n";
}
 
?>
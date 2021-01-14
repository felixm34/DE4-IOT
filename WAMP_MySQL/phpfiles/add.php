<?php
   	include("connect.php");
   	
   	$link=Connection();

	$humidity=$_POST["humidity"];
	$soilMoisture=$_POST["soilMoisture"];
	$light=$_POST["light"];
	$temperature=$_POST["temperature"];

	$query = "INSERT INTO `rawdata` (`timestamp`, `humidity`, `soilMoisture`, `light`, `temperature`) 
		VALUES ('".$humidity."','".$soilMoisture."', '".$light."', '".$temperature."')"; 
   	
   	mysqli_query($link, $query);
	mysqli_close($link);

   	header("Location: index.php");
?>

<?php

	function Connection(){
		$server="plantmonitor";
		$user="root";
		$pass="";
		$dbname="plantdata";
	   	
		$connection = mysqli_connect($server, $user, $pass, $dbname);

		if (!$connection) {
	    	die('MySQL ERROR: ' .mysqli_error($connection));
		}
		
		mysqli_select_db($connection, $dbname) or die( 'MySQL ERROR: '.mysqli_error($connection) );

		return $connection;
	}
?>

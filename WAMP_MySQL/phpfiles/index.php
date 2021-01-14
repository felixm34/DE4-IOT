<?php

	include("connect.php"); 	
	
	$link = Connection();

	$result = mysqli_query($link, "SELECT * FROM `rawdata` ORDER BY `timestamp` DESC");
?>

<html>
   <head>
      <title>Plant Health Monitor</title>
   </head>
<body>
   <h1>Raw data</h1>

   <table border="1" cellspacing="1" cellpadding="1">
		<tr>
			<td>&nbsp;Timestamp &nbsp;</td>
			<td>&nbsp;Humidity &nbsp;</td>
			<td>&nbsp;Soil Moisture &nbsp;</td>
			<td>&nbsp;Light &nbsp;</td>
			<td>&nbsp;Temperature &nbsp;</td>
		</tr>

      <?php 
		  if($result!==FALSE){
		     while($row = mysqli_fetch_array($result)) {
		        printf("<tr><td> &nbsp;%s </td><td> &nbsp;%s&nbsp; </td><td> &nbsp;%s&nbsp; </td><td> &nbsp;%s&nbsp; </td><td> &nbsp;%s&nbsp; </td></tr>", 
		           $row["timestamp"], $row["humidity"], $row["soilMoisture"], $row["light"], $row["temperature"]);
		     }
		     mysqli_free_result($result);
		     mysqli_close($link);
		  }
      ?>

   </table>
</body>
</html>

<?php

	$con = mysqli_connect('localhost', 'agrosensor', 'agro!ar');
	
	if (!$con) {
		echo "Not connected to Server" . " " . mysqli_connect_error();
		exit();
	}
	
	if (!mysqli_select_db($con, 'videos')) {
		echo "Database not selected" . " " . mysqli_connect_error();
		exit();
	}


?>
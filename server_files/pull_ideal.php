<?php
	
	session_start();

	$con = mysqli_connect('localhost', 'agrosensor', 'agro!ar');
	
	if (!$con) {
		echo "Not connected to Server" . " " . mysqli_connect_error();
		exit();
	}
	
	if (!mysqli_select_db($con, 'agrosensor')) {
		echo "Database not selected" . " " . mysqli_connect_error();
		exit();
	}
	
	$Name = $_GET['Name'];
	//$Name = "Chinese Evergreen";
	
	
	//Getting query and then outputting it HOPEFULLY LOL
	$query = "SELECT * FROM Ideal_Values WHERE Plant_Name = '$Name'";
	
	$result = $con->query($query); 
	
	
	$datas = $result->fetch_assoc(); 

	$json=json_encode($datas, true); 
	echo $json; //outputting the final json forma
	
	/*
	if (!mysqli_query($con, $sql)) {
		echo "Not Pulled" . " " . mysqli_connect_error();
		exit();
	} else {
		while ($row = $sql->fetch_object()){
			echo json_encode($row);
		};
		echo "Pulled";
	}
	*/


?>
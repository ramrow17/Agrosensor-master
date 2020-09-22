<?php

	$con = mysqli_connect('localhost', 'agrosensor', 'agro!ar');
	
	if (!$con) {
		echo "Not connected to Server" . " " . mysqli_connect_error();
		exit();
	}
	
	if (!mysqli_select_db($con, 'agrosensor')) {
		echo "Database not selected" . " " . mysqli_connect_error();
		exit();
	}
	
	$Name = "RTELL";
	$Air_Temp = 45.1;
	//$Soil_Temp = "44.0";
	$pH_Level = "90.1";
	$Soil_Moisture = "40.2";
	$Humidity = "40.3";
	$Date = date("md");
	$newDate = sprintf("%04s", $Date);
	
	echo $newDate;
	
	$sql = "INSERT INTO plant_measurements (Name, Air_Temp, pH_Level, Soil_Moisture, Humidity, Date) VALUES ('$Name', '$Air_Temp', '$pH_Level', '$Soil_Moisture', '$Humidity', '$newDate')";
	
	if (!mysqli_query($con, $sql)) {
		echo "Not Inserted" . " " . mysqli_connect_error();
		exit();
	} else {
		echo "Inserted";
	}
	

?>

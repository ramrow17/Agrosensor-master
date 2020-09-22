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
	
	$Name = $_GET['Name'];
	$Air_Temp = $_GET['Air_Temp'];
	//$Soil_Temp = $_GET['Soil_Temp'];
	$pH_Level = $_GET['pH_Level'];
	$Soil_Moisture = $_GET['Soil_Moisture'];
	$Humidity = $_GET['Humidity'];
	$Date = date("m/d");
	$newDate = sprintf("%05s", $Date);
	
	//echo $newDate;
	
	$sql = "INSERT INTO plant_measurements (Name, Air_Temp, pH_Level, Soil_Moisture, Humidity, Date) VALUES ('$Name', '$Air_Temp', '$pH_Level', '$Soil_Moisture', '$Humidity', '$newDate')";
	
	if (!mysqli_query($con, $sql)) {
		echo "Not Inserted" . " " . mysqli_connect_error();
		exit();
	} else {
		echo "Inserted";
	}
	// $sql = "INSERT INTO plant_measurements (Name, Air_Temp, Soil_Temp, pH_Level, Soil_Moisture) VALUES ('".$_GET["Name"]."', '".$_GET["Air_Temp"]."', '".$_GET["Soil_Temp"]."', '".$_GET["pH_Level"]."', '".$_GET["Soil_Moisture"]."')";

?>



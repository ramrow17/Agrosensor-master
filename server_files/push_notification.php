<?php
	
	$Plant = $_GET['Plant'];
	$Sensor = $_GET['Sensor'];
	$Flag = $_GET['Flag'];
	
	$alert_message;
	
	switch ($Sensor) {
		case 'Air_Temp':
			switch ($Flag) {
				case 0:
					echo "The air temperature for your " . $Plant . " is too low! Raise it!";
					$alert_message = "The air temperature for your " . $Plant . " is too low! Raise it!";
					break;
				case 1:
					echo "The air temperature for your " . $Plant . " is too high! Lower it!";
					$alert_message = "The air temperature for your " . $Plant . " is too high! Lower it!";
					break;
			}
			break;
		case 'pH_Level':
			switch ($Flag) {
				case 0:
					echo "The soil pH level for your " . $Plant . " is too low! Raise it!";
					$alert_message = "The soil pH level for your " . $Plant . " is too low! Raise it!";
					break;
				case 1:
					echo "The soil pH level for your " . $Plant . " is too high! Raise it!";
					$alert_message = "The soil pH level for your " . $Plant . " is too high! Raise it!";
					break;
			}
			break;
		case 'Soil_Moisture':
			switch ($Flag) {
				case 0:
					echo "Your " . $Plant . " needs to be watered!";
					$alert_message = "Your " . $Plant . " needs to be watered!";
					break;
				case 1:
					echo "Your " . $Plant . " is overwatered!";
					$alert_message = "Your " . $Plant . " is overwatered!";
					break;
			}
			break;
		case 'Humidity':
			switch ($Flag) {
				case 0:
					echo "The environment for your " . $Plant . " is too dry! Move it to a more humid location.";
					$alert_message = "The environment for your " . $Plant . " is too dry! Move it to a more humid location.";
					break;
				case 1:
					echo "The environment for your " . $Plant . " is too humid! Move it to a drier location.";
					$alert_message = "The environment for your " . $Plant . " is too humid! Move it to a drier location.";
					break;
			}
			break;
	}
	
	
    function send_notification ($tokens, $message)
    {
        $url = 'https://fcm.googleapis.com/fcm/send';
        $fields = array(
             'to' => $tokens,
             'data' => $message
            );

        $headers = array(
            'Authorization: key=' . "AAAA8Eu_6iE:APA91bG4V2oA4A_VCpJrsBde3sNZwDV17M1vDzzp9flX5lJRIpBGN1myPYLRqdJagDVf87KihmJyw_XQLNyjBj_OmWDRE2ooDEpcVTlePQSFhA2WNTBuyv8uTboFNL1M6kELDFM8YaQ0",
            'Content-Type: application/json'
            );

       $ch = curl_init();
       curl_setopt($ch, CURLOPT_URL, $url);
       curl_setopt($ch, CURLOPT_POST, true);
       curl_setopt($ch, CURLOPT_HTTPHEADER, $headers);
       curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
       curl_setopt ($ch, CURLOPT_SSL_VERIFYHOST, 0);
       curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false);
       curl_setopt($ch, CURLOPT_POSTFIELDS, json_encode($fields));
       $result = curl_exec($ch);
       if ($result === FALSE) {
           die('Curl failed: ' . curl_error($ch));
       }
       curl_close($ch);
       return $result;
    }

    $tokens = "dx583r-6Stu9t33ez0jRRH:APA91bGYP6pdBP6qV3IOYcyd4c-A5_ChgYiZvDrzccEdt9B0eVCN6tn3O7gvEemP-00y1B0KgyETNl5ICH_gSF7BTdw0YAWcW52Kvd8JHf9UfxN7IY1bnJraUZOK1BgL-tw2g6TJGqeW";

    $message = array("message" => $alert_message);
    $message_status = send_notification($tokens, $message);
    echo $message_status;
   


 ?>
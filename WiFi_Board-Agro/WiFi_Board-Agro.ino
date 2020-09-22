#include <Wire.h>
#include <SPI.h>
#include <WiFiNINA.h>

// Library used to unpack JSON data that is received by the server 
#include "jsmn.h"

// These libraries are used to convert the hex values into the base64 strings
// They were put in place to be able to read data from the camera image, 
// compacted into a base64 string and then pushed onto the database. 
#include "base64.h"
#include "cdecode.h"
#include "cencode.h"

// Transfer from Sensor Array board to WiFi board
#include "SerialTransfer.h" 
SerialTransfer myTransfer;


uint8_t HexBuffer[1024] = {}; 
uint8_t spot = 0; uint8_t event = 0;
String myString = "";


//Converts an array of bytes into a base64 string
String base64::encode(uint8_t * data, size_t length) {
    // base64 needs more size then the source data
    size_t size = ((length * 1.6f) + 1);
    char * buffer = (char *) malloc(size + 1);
    if(buffer) {
        base64_encodestate _state;
        base64_init_encodestate(&_state);
        int len = base64_encode_block((const char *) &data[0], length, &buffer[0], &_state);
        len = base64_encode_blockend((buffer + len), &_state);

        String daString = String(buffer);
        free(buffer);
        return daString;
    }
    return String("-FAIL-");
}

//char ssid[] = "";     // home network - Insert your unique ssid and its corresponding password
//char pass[] = "";
int status = WL_IDLE_STATUS;     // the Wifi radio's status

char server[] = "157.245.185.143"; // Used to contact our server for scripts
WiFiClient client; 

// Global variables for the sensors values
double Air_Temp = 0; 
double pH_Level = 0; 
double Humidity = 0;
double Soil_Moisture = 0;
uint8_t PrepData = 0; 

// Name of plant & Ideal values
char CompName[] = "Striped Dracaena";
//char CompName[] = "Chinese Evergreen"; 
double IdealAir[] = {0, 100};
double IdealSoil[] = {0, 100};
double IdealpH[] = {0, 100};
double IdealHum[] = {0, 100};
double IdealSoilMoi[] = {0, 100};


//This has potential to become a problem, you will probably have to declare a const char * to fit the parameters within the jsmn parser.
// You might also want to rename the value something like json_temp or whatever so you don't have to change the other code.
String json_temp = "{"; 


void setup() 
{
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  Serial1.begin(115200);
  myTransfer.begin(Serial1);
  
  
  while (!Serial) {}

  // checks for the WiFi module properly connected
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // gets stuck to prevent more failure
    while (true);
  }

  // attempting to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);
   //status = WiFi.begin(ssid); //comment this for guest network (eaglenet)
    // wait 10 seconds for connection:
    delay(15000);
  }
  
  Serial.println("You are now connected to the internet!"); 

}



//Main Code call functions based on what is needed...
void loop()
{
	  
  if(myTransfer.available())
  {
	  
	// The first value in the buffer will allow us to know which 
	// branch to use for data acquisition

    for(byte i = 0; i < myTransfer.bytesRead; i++)
      {
       		
		// pH Level Branch
		if(myTransfer.rxBuff[0] == 0x00) 
		{
		  if (i > 0)
          {  char temp = myTransfer.rxBuff[i]; myString += String(temp);   } 		  
          if (i == 5)
          {  pH_Level = myString.toDouble(); myString = ""; event++;  }
		}
		
		// Humidity Branch
		else if (myTransfer.rxBuff[0] == 0x01)
        {
          if (i > 0)
          { char temp = myTransfer.rxBuff[i];  myString += String(temp);  }
          if (i == 5)
          { Humidity = myString.toDouble(); myString = ""; event++; }
        }


		// Air Temperature Branch
        else if (myTransfer.rxBuff[0] == 0x02)
        {
          if (i > 0)
          {  char temp = myTransfer.rxBuff[i]; myString += String(temp); }	  
          if (i == 5)
          { Air_Temp = myString.toDouble(); myString = ""; event++; }
          
        }

		// Soil Moisture Branch
        else if (myTransfer.rxBuff[0] == 0x03)
        {
          if (i > 0)
          { char temp = myTransfer.rxBuff[i]; myString += String(temp);  }
          if (i == 5)
          { Soil_Moisture = myString.toDouble(); myString = ""; event++; }
          
        }	
		
		
		// Camera values Branch???????
    // *NOTE* Testing kept failing - should you wonder why this is here
		else if(myTransfer.rxBuff[0] == 0x0A)
		{

			
		}	

   }// For byte = buffer
  }// if myTransfer.avaiable()
  
  // Error exit for the Serial port transfer
  else if(myTransfer.status < 0)
  {
    Serial.print("ERROR: ");

    if(myTransfer.status == -1)
      Serial.println(F("CRC_ERROR"));
    else if(myTransfer.status == -2)
      Serial.println(F("PAYLOAD_ERROR"));
    else if(myTransfer.status == -3)
      Serial.println(F("STOP_BYTE_ERROR"));
  }
	
		
	// Checking if the 4 sensor values have been taken. 
	if (event > 3)
	{  Read_Measurements(); Serial.println(); event = 0; PrepData++;  }


 // Camera values reach max buffer - We will convert to string then push to server
 if (spot >= 1023)
 {	
  
	base64 obj; 	
	String SampleString = obj.encode(HexBuffer, spot);
	spot = 0;

	Serial.println("Here is the sample String: "); 
	Serial.println(SampleString); 
	Serial.println("Exiting for testing!"); 
	

 
 }

  // Lets do these functions every hour, 
  // to not continually open and close 
  // network command to send over. 
  // Use Metro.h for specific operations. 

	// Every 5 minutes, send data of 
  if ( (PrepData%5) == 1 )
  {
    Get_Values(); 
    Serial.println(); 
    Compare_Vals(); 
    Serial.println(); 
    sending_data();
    Serial.println(); 
	if (PrepData > 200)
	{ PrepData = 1; }
    PrepData++; 
    
  }


 
}


// This function will simply read values from the sensors to the Serial Monitor
void Read_Measurements()
{
  Serial.println("Here is the data received from Sesnor Array!");
  Serial.print("Soil Moisture: ");
  Serial.println(Soil_Moisture); 
  Serial.print("Air Temperature: ");
  Serial.println(Air_Temp);
  Serial.print("Humidity: ");
  Serial.println(Humidity);
  Serial.print("pH Levels: ");
  Serial.println(pH_Level); 
}


// Here is where we are going to make a request from the database
// Then parse the information given by the pull_ideal.php script
void Get_Values()
{

  
	Serial.println("Sending Request to receive values...");
  // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {
    Serial.println("connected to server");
    // Make a HTTP request:
    //You will change the name of the script so that way we get the correct values...
    client.print("GET /pull_ideal.php?Name=");     //YOUR URL - pull_ideal.php
    client.print(CompName);
    
    client.print(" ");      //SPACE BEFORE HTTP/1.1
    client.print("HTTP/1.1");
    client.println();
    client.print("Host: 157.245.185.143");
    client.println();
    
    client.println("Connection: close");
    client.println();

	}
	 
	
	 // Here we will read in the data given by the script and parse them accordingly
  // But first, we will be reading the incoming data from the http request from above!
  while(1)
  {
     // if there are incoming bytes available
    // from the server, read them and print them:
   while (client.available()) {
     char c = client.read();
     if (c == '{')
     { 
       while(client.available()) {
         char c = client.read();
         if (c == '}')
         { break; }
         else {json_temp += c;   }
       }
	       json_temp += "}";
     }    
  }

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    client.stop(); 
    Serial.println("Data from database acquired! Disconnecting...");


  // Here is where the real fun begins. 
 // We're going to be parsing the data within the json string provided
// Format it into a c-string, then convert the data into doubles to be used in the program.

  const char *JSON_STRING = json_temp.c_str();
  int i, j;
  int r;
  jsmn_parser p;
  jsmntok_t t[32]; // We expect no more than 32 tokens 

  jsmn_init(&p);
  r = jsmn_parse(&p, JSON_STRING, strlen(JSON_STRING), t,
                 sizeof(t) / sizeof(t[0]));
  if (r < 0) {
    printf("Failed to parse JSON: %d\n", r);
    return 1;
  }

  // Assume the top-level element is an object 
  if (r < 1 || t[0].type != JSMN_OBJECT) {
    printf("Object expected\n");
    return 1;
  }
  
  double values[12]; //declaring an array of doubles to be used in the values from the json string.
    
    i = 0; //used for iteration and boundary control in this for loop
    for (j = 0; j < 10; j++)
    {
      if (i < 20)
      {
        values[j] = atof( JSON_STRING + t[6+i].start); i+=2; //converting string to double
      }
    }   

    IdealAir[0] = values[4]; //Min Air Temp
    IdealAir[1] = values[0]; //Max Air Temp
	
    IdealHum[0] = values[5]; //Min Humidity Level
    IdealHum[1] = values[1]; //Max Humidity Level
    
    IdealpH[0] = values[6]; //Min pH Level
    IdealpH[1] = values[2]; //Max ph Level
        
    IdealSoilMoi[0] = values[7]; //Min Soil Moisture
    IdealSoilMoi[1] = values[3]; //Max Soil Moisture

    json_temp = "{"; //Resetting the string to original value 

     
  
    return; 
  
    } // After Client disconnects, perform parsing and storage 
  }  // Loop to ensure data is being pulled

 
} // End of Function - GetValues()




//Simple comparison of values based from the information given from the Ideal Values Database
void Compare_Vals()
{

  Serial.println("Testing out all the values against thresholds!"); 
  boolean daflag = false; 

  // If flag is 0, value is below threshold
  // Otherwise, flag is above threshold
  
	if (Air_Temp < IdealAir[0])
	{
		SendAlert("Air_Temp", 0); daflag = true; 
	}
	else if (Air_Temp > IdealAir[1])
	{
		SendAlert("Air_Temp", 1); daflag = true;
	}
		
	if (pH_Level < IdealpH[0])
	{
     SendAlert("pH_Level", 0); daflag = true;
	}
	else if (pH_Level > IdealpH[1])
	{
	  SendAlert("pH_Level", 1); daflag = true;
	}	
	
	if (Humidity < IdealHum[0])
	{
		SendAlert("Humidity", 0); daflag = true;
	}
	else if (Humidity > IdealHum[1])
	{
		SendAlert("Humidity", 1); daflag = true;
	}		
	
	if (Soil_Moisture < IdealSoilMoi[0])
	{
		SendAlert("Soil_Moisture", 0); daflag = true;
	}
	else if (Soil_Moisture > IdealSoilMoi[1])
	{
		SendAlert("Soil_Moisture", 1); daflag = true;
	}
	if (daflag == true)
 {	
	  Serial.println("Testing complete! Exiting..."); 
 }
 else { Serial.println("No issues with sensor values!"); }
  
  return; 
} // End of Function - Compare_Vals()




// Simple function to send an alert based on the values that are not
// within threshold limits.
void SendAlert(char* val, bool FlagVal)
{
  // If flag is 0, value is below threshold
  // Otherwise, flag is above threshold
  // Sends the Name of plant and the sensor that not within threshold.
 
	
	  if (client.connect(server, 80)) {
    Serial.print("Error found for "); Serial.println(val); 
    Serial.println("Sending alert to app!");
    // Make a HTTP request:
    client.print("GET /push_notification.php?Plant=");     //YOUR URL
    client.print(CompName);

    client.print("&Sensor=");
    client.print(val);

    client.print("&Flag=");
    client.print(FlagVal);

    client.print(" ");      //SPACE BEFORE HTTP/1.1
    client.print("HTTP/1.1");
    client.println();
    client.print("Host: 157.245.185.143");
    client.println();
    
    client.println("Connection: close");
    client.println();
  }
	
	while (client.available()) // Clearing client buffer
	 { char c = client.read(); }

   // if the server's disconnected, stop the client:
   if (!client.connected()) {
      client.stop();
	}
	
	Serial.println("Successfully sent alert, please check the mobile app for confirmation.");
	return; 
  
} // End of Function - SendAlert(char*, bool)


// This function uses a base64 string from data values, and the order
// of the whole string to line them together in the server. 
// *NOTE* Testing has failed, code will not be used.
// Params - Base64_String -> converted values from camera
//        - Iteration -> which section of the large picture base64 string
void SendingBase64_String(String Base64_String, uint8_t Iteration)
{
	
} // End of Function - SendingBase64_String(String, uint8_t)



// This function sends sensor data from the boards into the main database
// Data is sent in a RESTful manner - values are read from URL then parsed
// within the db_script.php script on server.
void sending_data()
{
    Serial.println("Pushing values onto Database!");
  // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {
    Serial.println("connected to server");
    // Make a HTTP request:
    client.print("GET /db_script.php?Name=");     //YOUR URL
    client.print(CompName);
    
    client.print("&Air_Temp=");
    client.print(Air_Temp);

    client.print("&pH_Level=");
    client.print(pH_Level);
	
	  client.print("&Humidity=");
    client.print(Humidity);
    
    client.print("&Soil_Moisture=");
    client.print(Soil_Moisture);

    client.print(" ");      //SPACE BEFORE HTTP/1.1
    client.print("HTTP/1.1");
    client.println();
    client.print("Host: 157.245.185.143");
    client.println();
    
    client.println("Connection: close");
    client.println();
  }

  while(1)
  {
    // if there are incoming bytes available
  // from the server, read them and print them:
  while (client.available()) {
    char c = client.read(); //Clearing client buffer
  }

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    client.stop();
    Serial.println("Values has been pushed to database - Please check your mobile app!"); 
    return; 
  } 

  }

} // End of Function - sending_data()

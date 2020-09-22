/*
* Slave Bluetooth
*
* The board with the slave bluetooth module is the one with all of the sensors.
* It is the HC-06 model and can only be configured into slave mode.  It has 4 pins,
* Vcc, Gnd, Rx, and Tx
*
* Sends temperature, humidity, moisture level, and pH level data 
* to the master bluetooth 
* 
* Uses the following pins
*
* D8 - software serial RX
* D9 - software serial TX
* D5 - Dht11 temp/humidity sensor
* A0 - Camera
* A1 - pH Sensor
* 
* When wiring the bluetooth to the arduino, match Rx to Tx and Tx to Rx.  We're
* just using digital pins 8 and 9 instead of the Rx and Tx on the board because
* we won't have to unplug those wires when uploading code every time.  Otherwise 
* we'd get an error if we didn't
*
* AltSoftSerial uses D9 for TX and D8 for RX. 
* 
* The slave device waits until it it gets the “sendData” request, it then reads 
* the values of the sensors and then sends it out as ascii via the software serial 
* channel. The sketches use the same recvWithStartEndMarkers() function as before 
* and so the data is wrapped in the < start marker and the > end marker.
* 
* Note on Metro Library: 
* 
* The metro library allows the sensors that control the temperature, humidity,
* and soil moisture to be read every so often without having to stop the program.
* This allows devices such as the pH sensor to be read in manually.
*/

#include <String.h>
#include <Metro.h>       // Timing library
#include "DHT.h"         // Library for the temp/humidity sensor (dht11)
#include "DFRobot_PH.h"  // Library for the pH sensor
#include <EEPROM.h>
#include <AltSoftSerial.h>
AltSoftSerial BTserial; 


#define DHTPIN 5        // Temp/humidity sensor is connected to pin 5
#define DHTTYPE DHT11   // DHT 11
#define PH_PIN A1       // Analog input for pH sensor

DHT dht(DHTPIN, DHTTYPE);    // Initialize DHT Sensor
DFRobot_PH ph;               // Initialize pH Sensor
Metro measure = Metro(3000); // Sensor measurements taken every 3 seconds
 
// Set DEBUG to true to output debug information to the serial monitor
boolean DEBUG = true;
 
// Variables used for incoming data
const byte maxDataLength = 20;
char receivedChars[21] ;
boolean newData = false;

int voltage,phValue,temperature = 25;  // Will be used for pH calculations
int soilmoisturepercent;
char byte_input = 0; // ascii selection from user
int choice = 0;      // Converts user's selection to integer desired
const int AirValue = 620;  
const int WaterValue = 310;  
int soilMoistureValue = 0;



 
void setup()  
{
    if (DEBUG)
    {
        // open serial communication for debugging
        Serial.begin(9600);
        Serial.println(__FILE__);
        Serial.println(" ");
    }
 
    //  open a software serial connection to the Bluetooth module.
    BTserial.begin(9600); 
    if (DEBUG)  
    {   
      Serial.println(F("AltSoftSerial started at 9600"));     
    }
    newData = false; 
    dht.begin();
    ph.begin();  // Allows the pH sensor to work when called
} 
 
 
 
 
void loop()  
{
/*
* This section, separated by *'s, is the incomplete part for reading the pH sensor.
* It's intended to be activated by entering a number into the serial monitor.  It
* shouldn't interfere with what you're doing because it's not setup to send the pH 
* value to the other arduino yet.  If for some reason it does just comment it out
*/

/*
************************************************************************************************************************************
*/
    // If user enters something in serial monitor, activate pH sensor
    if (Serial.available() > 0)
    {  
      byte_input = Serial.read();
      choice = byte_input - '0'; // Converts input into desired integer
      Serial.print("Number inputed: ");
      Serial.println(choice);
  
      // pH Sensor
      if (choice = 1) 
      {
        temperature = dht.readTemperature(true);         // read your temperature sensor to execute temperature compensation
        voltage = analogRead(PH_PIN)/1024.0*5000;  // read the voltage
        phValue = ph.readPH(voltage,temperature);  // convert voltage to pH with temperature compensation
        
        Serial.print("pH:");
        Serial.println(phValue,2);
        Serial.println();
    
        ph.calibration(voltage,temperature);           // calibration process by Serial CMD
      }
      Serial.println();
    }
/*
************************************************************************************************************************************
*/



    recvWithStartEndMarkers(); 
    
    // If 3 seconds have passed, read temperature, humidity, and soil moisture sensors
    if(measure.check())
    {
      if (newData)  
      { 
        processCommand(); 
      }
    }    
}
 
 



// Gets the soil moisture percentage
int getMoisture()
{
  // Moisture Sensor
  soilMoistureValue = analogRead(A0);
  soilmoisturepercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);
  
  return soilmoisturepercent;
}



/* 
* Parses data commands contained in receivedChars[]
* 
* The "if (strcmp ("sendData",receivedChars) == 0)" part checks for data. 
* If there's a start marker "<" it starts to put the recieved data in to the 
* variable receivedChars[].  When an end marker ">" is received it sets the newData 
* flag to TRUE. Any data not within the start and end markers is ignored.
* When newData is TRUE we know we have a command to process
*/
void processCommand()
{
  Serial.println(receivedChars);
  
  if (strcmp ("sendData",receivedChars) == 0) 
  { 
     int current_temp = dht.readTemperature(true);
     int humidity = dht.readHumidity();
     int moisture = getMoisture();

     // This was simply to test if we can send a string to the other board and 
     // we can as long as we include the String.h library
     String testing1 = "Hello World"; 

     // BTserial sends the data to the other board
     BTserial.print("<");  
     BTserial.print( current_temp ); 
     BTserial.print(">");

     BTserial.print("<");  
     BTserial.print( humidity ); 
     BTserial.print(">");

     BTserial.print("<");  
     BTserial.print( moisture ); 
     BTserial.print(">");

     BTserial.print("<");  
     BTserial.print( testing1 ); 
     BTserial.print(">");

     // This section just prints to the serial monitor of the slave device so we
     // know that data is being read in
     if (DEBUG) 
     { 
      Serial.print("Temp is ");   
      Serial.print(current_temp);  
      Serial.println(""); 

      Serial.print("Humidity is ");   
      Serial.print(humidity);  
      Serial.println("");  

      Serial.print("Moisture level is ");   
      Serial.print(moisture);  
      Serial.print("%");
      Serial.println();
      Serial.println();

      Serial.println(testing1);
      Serial.println();
      Serial.println();
     }
  }
  
  newData = false;
  receivedChars[0]='\0'; 
}
 
 
 
 
 
// Reads serial data and returns the content between a start marker and an 
// end marker.  Using these markers allows the receiving device to check
// that it has a full command before acting upon it
void recvWithStartEndMarkers()
{
  static boolean recvInProgress = false;
  static byte ndx = 0; // To read each byte one by one
  char startMarker = '<';
  char endMarker = '>';
  char rc; // Variable for reading in data from the slave
  
  if (BTserial.available() > 0) 
  {
    rc = BTserial.read();

    // If there is data coming in 
    if (recvInProgress == true) 
    {
       // If there is still data, keep on reading until endmarker ">" is reached
       if (rc != endMarker) 
       {
          receivedChars[ndx] = rc; // Read the next byte then go to the next one
          ndx++;
          
          if (ndx > maxDataLength) 
          { 
            ndx = maxDataLength; 
          }
       }
       else 
       {
           receivedChars[ndx] = '\0'; // terminate the string
           recvInProgress = false;
           ndx = 0;
           newData = true; // End marker received so set newData to true
       }
    }
    else if (rc == startMarker) // Puts the starting marker "<" at the beginning of the data
    { 
      recvInProgress = true; 
    }
  }
}

/*
* This is the source code for the Sensor Array on Team Agrosensor's Project.
* This code's primary role is to acquire all the data with the various sensors.
* Pack the data into a usable form, sent over to the WiFi board, and continously
* sent over and over - to be caught and unpack by the other board.
* 
* There are one more thing that needed to be implemented, however, there was an issue
* with reading data. There was trouble with running the camera in testing, though, 
* there is an executable that is able to display a picture that works without any issue.
* 
* 
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
* The metro library allows the sensors that control the temperature, humidity,
* and soil moisture to be read every so often without having to stop the program.
* This allows devices such as the pH sensor to be read in manually.
*/


// This is the sender code for the Serial Communication testing. 
#include <Metro.h>       // Timing library
#include "DHT.h"         // Library for the temp/humidity sensor (dht11)
#include "DFRobot_PH.h"  // Library for the pH sensor
#include <EEPROM.h>

#define DHTPIN 5        // Temp/humidity sensor is connected to pin 5
#define DHTTYPE DHT11   // DHT 11
#define PH_PIN A1       // Analog input for pH sensor

DHT dht(DHTPIN, DHTTYPE);    // Initialize DHT Sensor
DFRobot_PH ph;               // Initialize pH Sensor
Metro measure = Metro(3000); // Sensor measurements taken every 3 seconds
 
// Set DEBUG to true to output debug information to the serial monitor
boolean DEBUG = false;

int voltage,phValue,temperature = 25;  // Will be used for pH calculations
const int AirValue = 620;  
const int WaterValue = 310;

  //Assume this is where the values are being recorded from the sensors
  // Value 1 - pH 
  // Value 2 - Humidity
  // Value 3 - Air Temperature
  // Value 4 - Soil Moisture
   double value1 = 0, value2 = 0, value3 = 0, value4 = 0;


// Used for serial communication between boards. 
#include "SerialTransfer.h"
SerialTransfer myTransfer;

// Buffer for sending the values of the sensors properly to the WiFi board
byte ArrayBuff[8] = {};
unsigned int Sizebb = 8; 


void setup()
{
  Serial.begin(115200);
  myTransfer.begin(Serial);
  delay(1000); 

    dht.begin();
    ph.begin();  // Allows the pH sensor to work when called
  
}

void loop()
{
   temperature = dht.readTemperature(true);         // read your temperature sensor to execute temperature compensation
   voltage = analogRead(PH_PIN)/1024.0*5000;  // read the voltage
   value1 = ph.readPH(voltage,temperature);
  
    while(1)
    {       
      value2 = (double) dht.readHumidity();
      value3 = (double) dht.readTemperature(true);
      if( (value2 > 0) && (value3 > 0))
       break;      
    }
    double soil = analogRead(A0);
    value4 = map(soil, AirValue, WaterValue, 0, 100);
 
  
	// Now the fun stuff will begin...
	// We are going to be sending over the values of each sensor to the wifi board

    
  //Sending Value1
  uint8_t space = 0;  
  String DaString = String(value1, 2);
  DaString.getBytes(ArrayBuff, Sizebb); 

  myTransfer.txBuff[space] = 0x00; 
  space++;
  
  for (int i = 0; i < 5; i++)
  {
    myTransfer.txBuff[space] = ArrayBuff[i]; space++; 
  }
  
  myTransfer.txBuff[space] = 0xFF;
  myTransfer.sendData(space);
   DaString = ""; 


  

  //Sending Value2
  space = 0;  delay(1000);
  DaString = String(value2, 2);
  DaString.getBytes(ArrayBuff, Sizebb); 

  myTransfer.txBuff[space] = 0x01; 
  space++;
  
  for (int i = 0; i < 5; i++)
  {
    myTransfer.txBuff[space] = ArrayBuff[i]; space++; 
  }
  
  myTransfer.txBuff[space] = 0xFF;

  myTransfer.sendData(space);
  DaString = ""; 


  //Sending Value3 
  space = 0; delay(1000);
  DaString = String(value3, 2);
  DaString.getBytes(ArrayBuff, Sizebb); 

    myTransfer.txBuff[space] = 0x02; 
  space++;
  
  for (int i = 0; i < 5; i++)
  {
    myTransfer.txBuff[space] = ArrayBuff[i]; space++; 
  }
  
  myTransfer.txBuff[space] = 0xFF;
  myTransfer.sendData(space);
  DaString = ""; 

  //Sending Value4 
  space = 0; delay(1000);
  DaString = String(value4, 2);
  DaString.getBytes(ArrayBuff, Sizebb); 

    myTransfer.txBuff[space] = 0x03; 
  space++;
  
  for (int i = 0; i < 5; i++)
  {
    myTransfer.txBuff[space] = ArrayBuff[i]; space++; 
  }
  
  myTransfer.txBuff[space] = 0xFF;
  myTransfer.sendData(space);
  DaString = ""; 
  
  
  // Delay = 1 minute - 60,000 ms
  
  delay(60000); 
}

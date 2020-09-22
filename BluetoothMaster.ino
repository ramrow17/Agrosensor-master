/*
* Master Bluetooth
*
* Receives temperature, humidity, moisture level, and pH level data 
* from the slave bluetooth 
* 
* Uses the following pins
*
* D8 - software serial RX
* D9 - software serial TX
* D5 - Dht11 temp/humidity sensor
*
* When wiring the bluetooth to the arduino, match Rx to Tx and Tx to Rx.  We're
* just using digital pins 8 and 9 instead of the Rx and Tx on the board because
* we won't have to unplug those wires when uploading code every time.  Otherwise 
* we'd get an error if we didn't
*
* AltSoftSerial uses D9 for TX and D8 for RX. 
* 
* Note on Metro Library: 
* 
* The metro library allows the sensors that control the temperature, humidity,
* and soil moisture to be read every so often without having to stop the program.
* This allows devices such as the pH sensor to be read in manually.
*/

#include <Metro.h>       // Timing library
#include <String.h>
#include <AltSoftSerial.h>
AltSoftSerial BTserial; 

Metro measure = Metro(3000); // Sensor measurements taken every 3 seconds

// Set DEBUG to true to output debug information to the serial monitor
boolean DEBUG = true;
  
// Variables used for incoming data
const byte maxDataLength = 20;
char receivedChars[21] ;
boolean newData = false;
boolean run = true;

int count = 0;
String sensor;


 
void setup()  
{  
  if (DEBUG)
  {
     // open serial communication for debugging
     Serial.begin(9600);
  }
  BTserial.begin(9600); 
  newData = false; 
} 
 
 
 
 
void loop()  
{  
  if(measure.check())
  {
     BTserial.print("<sendData>");  
     if (DEBUG) 
     { 
        Serial.println();
        Serial.println("Request sent"); 
     }
  }

  recvWithStartEndMarkers();

  /*
  * This will read in one value at a time.  If you have three sensors reading in 
  * from the slave then this piece of code will need to run three times.  The first
  * value is temperature, the second is humidity, and the third is soil moisture
  */
  if (newData)  
  {   
     Serial.print("Sensor Value is: ");
     Serial.print(receivedChars);
     Serial.println();   

     newData = false;  
     receivedChars[0]='\0'; 
  }     
}
 
 
 
 
// Reads serial data and returns the content between a start marker and an 
// end marker.  Using these markers allows the receiving device to check
// that it has a full command before acting upon it
int recvWithStartEndMarkers()
{
   static boolean recvInProgress = false;
   static byte ndx = 0;  // To read each byte one by one
   char startMarker = '<';
   char endMarker = '>';
   char rc;  // Variable for reading in data from the slave

   if (BTserial.available() > 0) 
   {
      rc = BTserial.read();

      // If there is data coming in 
      if (recvInProgress == true) 
      {
         // If there is still data, keep on reading until endmarker ">" is reached
         if (rc != endMarker) 
         { 
            receivedChars[ndx] = rc;  // Read the next byte then go to the next one
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

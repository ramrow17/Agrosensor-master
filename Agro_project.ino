// This code manages the sensor readings from the Arduino Uno

/*
 * Note on Metro Library: 
 * 
 * The metro library allows the sensors that control the temperature, humidity,
 * and soil moisture to be read every so often without having to stop the program.
 * This allows devices such as the pH sensor to be read in manually.
*/

#include <Metro.h>       // Timing library
#include "DHT.h"         // Library for the temp/humidity sensor (dht11)
#include "DFRobot_PH.h"  // Library for the pH sensor
#include <EEPROM.h>

#define DHTPIN 5        // Temp/humidity sensor is connected to pin 5
#define DHTTYPE DHT11   // DHT 11
#define PH_PIN A1       // Analog input for pH sensor

DHT dht(DHTPIN, DHTTYPE);    // Initialize DHT Sensor
DFRobot_PH ph;               // Initialize pH Sensor
Metro measure = Metro(5000); // Sensor measurements taken every 5 seconds


float voltage,phValue,temperature = 25;
float soilmoisturepercent;
char byte_input = 0; // ascii selection from user
int choice = 0;      // Converts user's selection to integer desired
const int AirValue = 620;   //you need to replace this value with Value_1
const int WaterValue = 310;  //you need to replace this value with Value_2
int soilMoistureValue = 0;




void setup()
{ 
  Serial.begin(9600);
  dht.begin();
  ph.begin();
}





void loop()
{
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




  // If 5 seconds have passed, read temperature, humidity, and soil moisture sensors
  if(measure.check())
  {
    delay(1000);
    int current_temp = dht.readTemperature(true);
    delay(250);
    int humidity = dht.readHumidity();

    Serial.println();
    Serial.println();
    Serial.print("Temperature = ");
    Serial.println(current_temp);  // read in fahrenheit instead of celsius
    Serial.print("Humidity = ");
    Serial.println(humidity);

    delay(250); // Slight delay needed to make sure moisture is read in time

    // Moisture Sensor
    soilMoistureValue = analogRead(A0);  //put Sensor insert into soil
    soilmoisturepercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);
    
    if(soilmoisturepercent > 100)
    {
      Serial.println("100 %");
    }
    else if(soilmoisturepercent < 0)
    {
      Serial.println("0 %");
    }
    else if(soilmoisturepercent > 0 && soilmoisturepercent < 100)
    {
      Serial.print("Moisture = ");
      Serial.print(soilmoisturepercent);
      Serial.println("%"); 
      Serial.println(); 
    }
  }
}
  

#include "thingProperties.h"

#include <SPI.h>
#include <WiFiNINA.h>
#include <WiFiSSLClient.h>
#include <RTCZero.h>
#include <AnalogPin.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 3
#define DHTTYPE DHT22   // DHT 22  (AM2302)

DHT dht(DHTPIN, DHTTYPE);
  
// Define analog pins for smoothing library
AnalogPin moistureSensor(A1);
AnalogPin lightSensor(A2);

// Define soil moisture sensor sensitivity (average min and max vals)
const int dryValue = 680;
const int wetValue = 280;

// Define light sensor sensitivity (average min and max vals)
const int brightValue = 600;
const int darkValue = 1023;

/* Comment Serial.print statements so that the code can run on a standalone board (not connected to computer) */

void setup() {
//  Serial.begin(9600);
//   while (!Serial) {
//    ; // wait for serial port to connect
//  }

  // Set up properties in Arduino Cloud
  initProperties();

  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  // Set up dht sensor
  pinMode(2, INPUT);
  dht.begin();

//  Serial.println("Setup complete.");
}

void loop() { 

  ArduinoCloud.update(); 
//  Serial.println("Arduino Cloud updated with values: ");
  
  // Read from soil moisture sensor, smooth using AnalogPin library 
  moistureSensor.setSmoothWeight(4);
  float soilMoistureValue = moistureSensor.readSmoothed();
  //  Serial.print(soilMoistureValue);
  // Map to % based on calibrated minmax
  soilMoisturePercent = map(soilMoistureValue, dryValue, wetValue, 0, 100);
//  Serial.print("Soil Moisture: ");
  if(soilMoisturePercent >= 100) {
//    Serial.println(100);
    soilMoisturePercent = 100;
  }
  else if(soilMoisturePercent <=0) {
//    Serial.println(0);
    soilMoisturePercent = 0;
  }
  else if(soilMoisturePercent > 0 && soilMoisturePercent < 100) {
//    Serial.println(soilMoisturePercent);
    soilMoisturePercent = soilMoisturePercent;
  }
  
  // Read from analog light sensor, smooth using AnalogPin library
  lightSensor.setSmoothWeight(4);
  float lightValue = lightSensor.readSmoothed();
  //  Serial.print(lightValue);
//  Serial.print("Light: ");
  // Map to % based on calibrated minmax
  lightPercent = map(lightValue, darkValue, brightValue, 0, 100);
  if(lightPercent >= 100) {
//    Serial.println(100);
    lightPercent = 100;
  }
  else if(lightPercent <=0) {
//    Serial.println(0);
    lightPercent = 0;
  }
  else if(lightPercent > 0 && lightPercent < 100) { 
//    Serial.println(lightPercent);
    lightPercent = lightPercent;
  }
  
  // Read from temp & humidity sensor
  humidityPercent = dht.readHumidity();
  temperature = dht.readTemperature();

//  Serial.print("Humidity: ");
//  Serial.println(humidityPercent);
//  Serial.print("Temperature: ");
//  Serial.println(temperature);
//  Serial.println(" ");

  delay(30000); // wait 30 seconds between updates
}

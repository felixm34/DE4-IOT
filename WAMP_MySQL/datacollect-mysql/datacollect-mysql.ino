#include "thingProperties.h"

#include <SPI.h>
#include <WiFiNINA.h>
#include <AnalogPin.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 3
#define DHTTYPE DHT22   // DHT 22  (AM2302)

DHT dht(DHTPIN, DHTTYPE);

int status = WL_IDLE_STATUS;

WiFiClient client;
char server[] = "http://plantmonitor/";
//IPAddress server(127, 0, 0, 1);
  
// Define analog pins for smoothing library
AnalogPin moistureSensor(A1);
AnalogPin lightSensor(A2);

// Define soil moisture sensor sensitivity (average min and max vals)
const int dryValue = 680;
const int wetValue = 280;

// Define light sensor sensitivity (average min and max vals)
const int brightValue = 600;
const int darkValue = 1023;

/* Comment Serial.print statements if the code is required to run on a standalone board (not connected to computer) */

void setup() {
//  Serial.begin(9600);
//  while (!Serial) {
//    ; // wait for serial port to connect
//  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
//    Serial.print("Attempting to connect to WPA SSID: ");
//    Serial.println(SSID);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(SSID, PASS);
  }

//  Serial.print("Connected to WPA SSID: ");
//  Serial.println(SSID);

  // Print Arduino's IP address:
  IPAddress ip = WiFi.localIP();
//  Serial.print("IP Address: ");
//  Serial.println(ip);

  // Set up properties in Arduino Cloud
  initProperties();

  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  // Set up dht sensor
  pinMode(2, INPUT);
  dht.begin();

  // Wait for response from wifi connections
  delay(5000);

//  Serial.println("Setup complete.");
}

void loop() { 

  // Update Arduino IoT dashboard with values
  ArduinoCloud.update(); 

    // Read from soil moisture sensor, smooth using AnalogPin library 
  moistureSensor.setSmoothWeight(4);
  float soilMoistureValue = moistureSensor.readSmoothed();
  // Serial.print(soilMoistureValue);
  // Map to % based on calibrated minmax
  soilMoisturePercent = map(soilMoistureValue, dryValue, wetValue, 0, 100);
  // Serial.print("Soil Moisture: ");
  if(soilMoisturePercent >= 100) {
    // Serial.println(100);
    soilMoisturePercent = 100;
  }
  else if(soilMoisturePercent <=0) {
    // Serial.println(0);
    soilMoisturePercent = 0;
  }
  else if(soilMoisturePercent > 0 && soilMoisturePercent < 100) {
    // Serial.println(soilMoisturePercent);
    soilMoisturePercent = soilMoisturePercent;
  }
  
  // Read from analog light sensor, smooth using AnalogPin library
  lightSensor.setSmoothWeight(4);
  float lightValue = lightSensor.readSmoothed();
  // Serial.print(lightValue);
  // Serial.print("Light: ");
  // Map to % based on calibrated minmax
  lightPercent = map(lightValue, darkValue, brightValue, 0, 100);
  if(lightPercent >= 100) {
    // Serial.println(100);
    lightPercent = 100;
  }
  else if(lightPercent <=0) {
    // Serial.println(0);
    lightPercent = 0;
  }
  else if(lightPercent > 0 && lightPercent < 100) { 
    // Serial.println(lightPercent);
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

  String data = "humidity=" + String(humidityPercent) + "&soilMoisture=" + String(soilMoisturePercent) + "&lightPercent=" + String(lightPercent) + "&temperature=" + String(temperature);
  // Serial.println(data);

  // Create a HTTP POST request using the formatted output of the sensor data
  httpRequest(data);

  // Wait 30 seconds between updates
  delay(30000); 
}

void httpRequest(String data) {
  // Close any connection before sending a new request
  client.stop();
  
//  Serial.println("\nStarting connection...");
  if (client.connect(server, 8080)) {
//    Serial.println("Server connected!");
    client.println("POST /add.php HTTP/1.1");
    client.println("Host: plantmonitor"); 
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.print("Content-Length: ");
    client.println(data.length());
    client.println();
    client.print(data);
  }
//  else {
//    Serial.println("Connection to server failed!");
//  }
}

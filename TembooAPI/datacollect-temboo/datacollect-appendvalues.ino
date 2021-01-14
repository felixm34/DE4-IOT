#include <SPI.h>
#include <WiFiNINA.h>
#include <WiFiSSLClient.h>
#include <TembooSSL.h>
#include "TembooAccount.h" // Contains Temboo account information
#include <RTCZero.h>
#include <AnalogPin.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 3
#define DHTTYPE DHT22   // DHT 22  (AM2302)

WiFiSSLClient client;
DHT dht(DHTPIN, DHTTYPE);
RTCZero rtc;

// Execution count, so this doesn't run forever
int calls = 1;  
// Maximum number of times the Choreo should be executed
int maxCalls = 60480; /* roughly once every 10 seconds for 7 days, 24/7 -
                         the server takes between 6 & 10 seconds to respond and input 
                         an entry to Google Sheets (dependent on wifi load) */
  
// Define analog pins for smoothing library
AnalogPin moistureSensor(A1);
AnalogPin lightSensor(A2);

// Define soil moisture sensor sensitivity (average min and max vals)
const int dryValue = 680;
const int wetValue = 280;

// Define light sensor sensitivity (average min and max vals)
const int brightValue = 600;
const int darkValue = 1023;

// Set start time of program
const byte seconds = 0;
const byte minutes = 50;
const byte hours = 16;
const byte day = 1;
const byte month = 1;
const byte year = 21;

// Create date & time stamps
char formattedDate[32]; 
char formattedTime[32];

void setup() {
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect
  }

  int wifiStatus = WL_IDLE_STATUS;

  // Determine if the WiFi Shield is present
  Serial.print("Shield:");
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("FAIL");

    // If there's no WiFi shield, stop here
    while(true);
  }

  Serial.println("OK");

  // Try to connect to the local WiFi network
  while(wifiStatus != WL_CONNECTED) {
    Serial.print("WiFi:");
    wifiStatus = WiFi.begin(WIFI_SSID, WPA_PASSWORD);

    if (wifiStatus == WL_CONNECTED) {
      Serial.println("OK");
    } else {
      Serial.println("FAIL");
    }
    delay(5000);
  } 

  // Set program start time
  rtc.begin();
  rtc.setTime(hours, minutes, seconds);
  rtc.setDate(day, month, year);

  // Set up dht sensor
  pinMode(2, INPUT);
  dht.begin();

  Serial.println("Setup complete.");
}

void loop() {
  
  if (calls <= maxCalls) {
    Serial.println("Running AppendValues - Run #" + String(calls++));

    // Necessary to manually input because MKR1010 board uses SAMD architecture and therefore other timekeeping libraries not configured
    sprintf(formattedDate, "%02u/%02u/%02u", rtc.getDay(), rtc.getMonth(), rtc.getYear());
    sprintf(formattedTime, "%02u:%02u:%02u", rtc.getHours(), rtc.getMinutes(), rtc.getSeconds());
    
    TembooChoreoSSL AppendValuesChoreo(client);

    // Invoke the Temboo client
    AppendValuesChoreo.begin();

    // Set Temboo account credentials
    AppendValuesChoreo.setAccountName(TEMBOO_ACCOUNT);
    AppendValuesChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
    AppendValuesChoreo.setAppKey(TEMBOO_APP_KEY);
    AppendValuesChoreo.setDeviceType(TEMBOO_DEVICE_TYPE);

    // Set Choreo inputs
    String RefreshTokenValue = "1//0dj-qpZ_-GlwdCgYIARAAGA0SNwF-L9IrHOyLmiQJmVNrWRN7ussDZM7LBMPRlP5QtRp2zhsFQtpH_zWZMdvWExraK2jUEHvqQrU";
    AppendValuesChoreo.addInput("RefreshToken", RefreshTokenValue);
    String ClientSecretValue = "qBUcbluq7Fi2GDpW0j-Pu7wh";
    AppendValuesChoreo.addInput("ClientSecret", ClientSecretValue);

    // Read from soil moisture sensor, smooth using AnalogPin library 
    moistureSensor.setSmoothWeight(4);
    float soilMoistureValue = moistureSensor.readSmoothed();
    //  Serial.print(soilMoistureValue);
    // Map to % based on calibrated minmax
    float soilMoisturePercent = map(soilMoistureValue, dryValue, wetValue, 0, 100);
    if(soilMoisturePercent >= 100) {
      Serial.print(100);
      soilMoisturePercent = 100;
    }
    else if(soilMoisturePercent <=0) {
      Serial.print(0);
      soilMoisturePercent = 0;
    }
    else if(soilMoisturePercent > 0 && soilMoisturePercent < 100) {
      Serial.print(soilMoisturePercent);
      soilMoisturePercent = soilMoisturePercent;
    }
    
    // Read from analog light sensor, smooth using AnalogPin library
    lightSensor.setSmoothWeight(4);
    float lightValue = lightSensor.readSmoothed();
    //  Serial.print(lightValue);
    Serial.print(" "); // necessary to separate single-line values for serial plotter
    // Map to % based on calibrated minmax
    float lightPercent = map(lightValue, darkValue, brightValue, 0, 100);
    if(lightPercent >= 100) {
      Serial.print(100);
      lightPercent = 100;

    }
    else if(lightPercent <=0) {
      Serial.print(0);
      lightPercent = 0;
    }
    else if(lightPercent > 0 && lightPercent < 100) { 
      Serial.print(lightPercent);
      lightPercent = lightPercent;
    }
    
    // Read from temp & humidity sensor
    float humidityPercent = dht.readHumidity();
    float temperature = dht.readTemperature();
  
    Serial.print(" ");
    Serial.print(humidityPercent);
    Serial.print(" ");
    Serial.println(temperature);

    String ValuesValue = "[[\"" + (String) formattedDate + "\", \"" + (String) formattedTime + "\", \"" + (String) temperature + "\", \"" + (String) humidityPercent + "\", \"" + (String) lightPercent + "\", \"" +  (String) soilMoisturePercent + "\"]]";
    AppendValuesChoreo.addInput("Values", ValuesValue);
    String ClientIDValue = "162504947063-o1pfbsqhb6dg9ljtggnok6m27d4q12f7.apps.googleusercontent.com";
    AppendValuesChoreo.addInput("ClientID", ClientIDValue);
    String SpreadsheetIDValue = "1MQUjcQN1iKwmm-Al67KpCK_GoEygKNB78dx1ml4X1w0";
    AppendValuesChoreo.addInput("SpreadsheetID", SpreadsheetIDValue);

    // Identify the Choreo to run
    AppendValuesChoreo.setChoreo("/Library/Google/Sheets/AppendValues");

    // Run the Choreo; when results are available, print them to serial
    AppendValuesChoreo.run();

    while(AppendValuesChoreo.available()) {
      char c = AppendValuesChoreo.read();
      Serial.print(c);
    }
    AppendValuesChoreo.close();
  }

  Serial.println("Waiting...");
  
}

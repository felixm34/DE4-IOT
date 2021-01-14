#include "arduino_secrets.h"
#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>

const char THING_ID[] = "8d424013-2078-4897-9403-43039584a401";

const char SSID[] = SECRET_SSID;    // Network SSID (name)
const char PASS[] = SECRET_PASSWORD;    // Network password (use for WPA, or use as key for WEP)

float lightPercent;
float humidityPercent;
float soilMoisturePercent;
float temperature;

void initProperties(){
  ArduinoCloud.setThingId(THING_ID);
  ArduinoCloud.addProperty(lightPercent, READ, 30 * SECONDS, NULL);
  ArduinoCloud.addProperty(humidityPercent, READ, 30 * SECONDS, NULL);
  ArduinoCloud.addProperty(soilMoisturePercent, READ, 30 * SECONDS, NULL);
  ArduinoCloud.addProperty(temperature, READ, 30 * SECONDS, NULL);
}

WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);

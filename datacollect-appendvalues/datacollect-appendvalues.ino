#include <SPI.h>
#include <WiFi101.h>
#include <WiFiSSLClient.h>
#include <TembooSSL.h>
#include "TembooAccount.h" // Contains Temboo account information

WiFiSSLClient client;

int calls = 1;   // Execution count, so this doesn't run forever
int maxCalls = 10;   // Maximum number of times the Choreo should be executed

void setup() {
  Serial.begin(9600);
  
  // For debugging, wait until the serial console is connected
  delay(4000);
  while(!Serial);

  int wifiStatus = WL_IDLE_STATUS;

  // Determine if the WiFi Shield is present
  Serial.print("\n\nShield:");
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

  Serial.println("Setup complete.\n");
}

void loop() {
  if (calls <= maxCalls) {
    Serial.println("Running AppendValues - Run #" + String(calls++));

    TembooChoreoSSL AppendValuesChoreo(client);

    // Invoke the Temboo client
    AppendValuesChoreo.begin();

    // Set Temboo account credentials
    AppendValuesChoreo.setAccountName(TEMBOO_ACCOUNT);
    AppendValuesChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
    AppendValuesChoreo.setAppKey(TEMBOO_APP_KEY);
    AppendValuesChoreo.setDeviceType(TEMBOO_DEVICE_TYPE);

    // Set Choreo inputs
    String RefreshTokenValue = "1//0dViiKtQPOgXyCgYIARAAGA0SNwF-L9IrDHHhobMtyS51AhFk9dtgBtITDCyu0u-9whTaV8tXqz7vyzOg-ymkyROfPx9UVUpSRkU";
    AppendValuesChoreo.addInput("RefreshToken", RefreshTokenValue);
    String ClientSecretValue = "fjk_x_J5xuehSHU0xgF5T1xT";
    AppendValuesChoreo.addInput("ClientSecret", ClientSecretValue);
    String ValuesValue = "[\n [\n  67343,\n  152\n ]\n]";
    AppendValuesChoreo.addInput("Values", ValuesValue);
    String ClientIDValue = "162504947063-dlesj6eufb64mmru56b4uehjfp1p2ebj.apps.googleusercontent.com";
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

  Serial.println("\nWaiting...\n");
  delay(30000); // wait 30 seconds between AppendValues calls
}
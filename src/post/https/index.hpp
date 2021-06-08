#include <Arduino.h>
#include <ArduinoJson.h>
#include <config.h>

// import ESP8266 built in libraries
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

ESP8266WiFiMulti WiFiMulti;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(WIFI_SSID, WIFI_PASSOWRD);

// put your main code here, to run repeatedly:
  Serial.println("Script Started!");
}

void loop() {


  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);

    DynamicJsonDocument dataJSON(1024);

    dataJSON["data"] = "hello";

    String data;
    serializeJson(dataJSON, data);

    client->setInsecure();

    HTTPClient https;

    Serial.print("[HTTPS] begin...\n");

    if (https.begin(*client, "https://")) {  // HTTPS
      Serial.print("[HTTPS] POST...\n");

      https.addHeader("Content-Type", "application/json");  //Specify content-type header
      // start connection and send HTTP header
      int httpCode = https.POST(data);

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] POST... code: %d\n", httpCode);

        // success saved to db
        if (httpCode == HTTP_CODE_OK) {
          // Serial.print("SUCESS!");
          String payload = https.getString();
          Serial.println("Payload: ");
          Serial.println(payload); 
        }
      } else {
        Serial.printf("[HTTPS] POST... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }
      
    } else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
  }

  Serial.println("Wait 10s before next round...");
  delay(10000);
}
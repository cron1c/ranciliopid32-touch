#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <SPI.h>
#include "userConfig.h"
#define FORMAT_SPIFFS_IF_FAILED true

bool loadConfig() {
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    Serial.println("Failed to open config file");
    return false;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("Config file size is too large");
    return false;
  }

  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);

  // We don't use String here because ArduinoJson library requires the input
  // buffer to be mutable. If you don't use ArduinoJson, you may as well
  // use configFile.readString instead.
  configFile.readBytes(buf.get(), size);

  StaticJsonDocument<256> doc;
  auto error = deserializeJson(doc, buf.get());
  if (error) {
    Serial.println("Failed to parse config file");
    return false;
  }

  const char* ssid = doc["D_SSID"];
  const char* pass = doc["PASS"];
  const char* auth = doc["AUTH"];
  const char* OTAhost = doc["OTAHOST"];
  const char* OTApass = doc["OTAPASS"];
  int TSICPIN = doc["TPIN"];
  const char* blynkaddress = doc["BLYNKADDRESS"];
  // Real world application would store these values in some variables for
  // later use.

  Serial.print("Loaded serverName: ");
  Serial.println(ssid);
  Serial.print("Loaded pass: ");
  Serial.println(pass);
  Serial.print("Loaded auth: ");
  Serial.println(auth);
  Serial.print("Loaded OTA: ");
  Serial.println(OTAhost);
  Serial.print("Loaded TPIN: ");
  Serial.println(TSICPIN);
  return true;
}

bool saveConfig() {
  StaticJsonDocument<256> doc;
  doc["D_SSID"] = D_SSID;
  doc["PASS"] = PASS;
  doc["AUTH"] = AUTH;
  doc["OTAHOST"] = OTAHOST;
  doc["OTAPASS"] = OTAPASS;
  doc["TPIN"] = TPIN;
  doc["BLYNKADDRESS"] = BLYNKADDRESS;
  
  
  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    return false;
  }

  serializeJson(doc, configFile);
  return true;
}

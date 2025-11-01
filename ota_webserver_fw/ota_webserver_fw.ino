#include <WiFi.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <EEPROM.h>

#define EEPROM_SIZE 4          // need at least 4 bytes for int
#define VERSION_ADDR 0         // store an int at address 0

WiFiClient client;

int storedVersion;
int latestVer;

// ======== Change WiFi credentials ========
const char* ssid = "Suman_Hathway";
const char* password = "H54AG8#f2CrpR4";

// ===== GitHub RAW URLs =====
String versionURL = "https://raw.githubusercontent.com/sumanpal129920/ota/main/Fw_version.txt";
String binURL     = "https://raw.githubusercontent.com/sumanpal129920/ota/main/firmware.bin?raw=true";
                     

// ================== EEPROM ==================
int readStoredVersion() {
  EEPROM.begin(EEPROM_SIZE);
  int v = EEPROM.readInt(VERSION_ADDR);
  EEPROM.end();
  return v;
}

void writeStoredVersion(int v) {
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.writeInt(VERSION_ADDR, v);
  EEPROM.commit();
  EEPROM.end();
}

// ================== WiFi ==================
void connectWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");
}

// ================== Version Check and Update ==================
void checkForUpdate()
{
  HTTPClient http;
  http.begin(versionURL);
  if (http.GET() == 200) {
    latestVer = http.getString().toInt();
    Serial.printf("Latest Version: %d\n", latestVer);

    if (latestVer > storedVersion) {
      Serial.println("New version available → updating...");
      doOTA();
    } else {
      Serial.println("Already latest.");
    }
  }
  http.end();
}

void doOTA()
{
  HTTPClient http;
  http.begin(binURL);

  int httpCode = http.GET();
  if (httpCode == 200) {
    int len = http.getSize();
    WiFiClient *client = http.getStreamPtr();

    if (!Update.begin(len)) return;
    Update.writeStream(*client);

    if (Update.end()) {
      writeStoredVersion(latestVer);      
      Serial.println("OTA SUCCESS → RESTARTING");
      ESP.restart();
    } else {
      Update.printError(Serial);
    }
  }
  http.end();
}

// ================== SETUP ==================
void setup() {
  Serial.begin(115200);
  delay(1000);

  connectWiFi();

  storedVersion = readStoredVersion();
  Serial.print("Stored Version: ");
  Serial.println(storedVersion);

  checkForUpdate();

  // ======== Your Application Code Init =======

}

// ================== LOOP ==================
void loop() {
  // ======== Your main program ========
  Serial.println("test3");
  delay(1000);
}

#include <ESP8266WiFi.h>
#include <EEPROM.h>

#define EEPROM_SIZE 64

void saveCredentials(const char* ssid, const char* password);
void readCredentials(char* ssid, char* password);
void listNetworks();

void setup() {
  Serial.begin(9600);
  while (!Serial) {}

  char storedSSID[32];
  char storedPassword[32];
  readCredentials(storedSSID, storedPassword);

  if (strlen(storedSSID) > 0 && strlen(storedPassword) > 0) {
    Serial.println("");
    Serial.println("Stored WiFi credentials found. Connecting...");
    WiFi.begin(storedSSID, storedPassword);
    int attempt = 0;
    while (WiFi.status() != WL_CONNECTED && attempt < 10) {
      delay(1000);
      Serial.print(".");
      attempt++;
    }
    if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected");
    Serial.print("Wifi Status: ");
    Serial.println(WiFi.status());
    Serial.print("Wifi Strength: ");
    Serial.println(WiFi.RSSI());
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("ESP Board MAC Address: ");
    Serial.println(WiFi.macAddress());


    } else {
      Serial.println("\nWiFi connection failed. Restarting...");
      delay(2000);
      ESP.restart();
    }
  } else {
    Serial.println("");
    Serial.println("No stored WiFi credentials found. Scanning available networks:");
    listNetworks();
    Serial.println("Please provide WiFi credentials:");
    Serial.println("Enter WiFi SSID:");
    while (!Serial.available()) {}
    String ssid = Serial.readStringUntil('\n');
    ssid.trim();
    Serial.println("Enter WiFi Password:");
    while (!Serial.available()) {}
    String password = Serial.readStringUntil('\n');
    password.trim();
    
    saveCredentials(ssid.c_str(), password.c_str());
    
    Serial.println("WiFi credentials saved. Rebooting...");
    delay(2000);
    ESP.restart();
  }
}

void loop() {}

void saveCredentials(const char* ssid, const char* password) {
  EEPROM.begin(EEPROM_SIZE);
  for (int i = 0; i < 32; ++i) {
    if (ssid[i] == '\0') break;
    EEPROM.write(i, ssid[i]);
  }
  for (int i = 0; i < 32; ++i) {
    if (password[i] == '\0') break;
    EEPROM.write(32 + i, password[i]);
  }
  EEPROM.commit();
}

void readCredentials(char* ssid, char* password) {
  EEPROM.begin(EEPROM_SIZE);
  for (int i = 0; i < 32; ++i) {
    ssid[i] = EEPROM.read(i);
    if (ssid[i] == '\0') break;
  }
  for (int i = 0; i < 32; ++i) {
    password[i] = EEPROM.read(32 + i);
    if (password[i] == '\0') break;
  }
}

void listNetworks() {
  int networks = WiFi.scanNetworks();
  Serial.println("Available networks:");
  for (int i = 0; i < networks; i++) {
    Serial.print(WiFi.SSID(i));
    Serial.print(" (");
    Serial.print(WiFi.RSSI(i));
    Serial.println(" dBm)");
  }
}

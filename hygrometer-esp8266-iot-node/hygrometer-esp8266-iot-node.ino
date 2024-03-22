#include <ESP8266WiFi.h>
#include <EEPROM.h>

// Define the size of EEPROM to store WiFi credentials
#define EEPROM_SIZE 64

// Structure to hold WiFi credentials
struct WiFiCredentials {
  char ssid[32];
  char password[32];
};

// Function prototypes
void setupWiFi();
void saveCredentials(const char* ssid, const char* password);
void readCredentials(char* ssid, char* password);

void setup() {
  Serial.begin(9600);
  while (!Serial) {} // Wait for Serial to be ready

  // Attempt to read WiFi credentials from EEPROM
  char storedSSID[32];
  char storedPassword[32];
  readCredentials(storedSSID, storedPassword);

  // If stored credentials found, try to connect
  if (strlen(storedSSID) > 0 && strlen(storedPassword) > 0) {
    Serial.println("Stored WiFi credentials found. Connecting...");
    setupWiFi();
  } else {
    Serial.println("No stored WiFi credentials found. Please provide WiFi credentials:");
    Serial.println("Enter WiFi SSID:");
    while (!Serial.available()) {}
    String ssid = Serial.readStringUntil('\n');
    ssid.trim();
    Serial.println("Enter WiFi Password:");
    while (!Serial.available()) {}
    String password = Serial.readStringUntil('\n');
    password.trim();
    
    // Save provided credentials to EEPROM
    saveCredentials(ssid.c_str(), password.c_str());
    
    Serial.println("WiFi credentials saved. Rebooting...");
    delay(2000);
    ESP.restart();
  }
}

void loop() {
  // Nothing to do here
}

void setupWiFi() {
  // Attempt to connect using stored credentials
  char storedSSID[32];
  char storedPassword[32];
  readCredentials(storedSSID, storedPassword);
  
  WiFi.begin(storedSSID, storedPassword);
  Serial.println("Connecting to WiFi...");

  int attempt = 0;
  while (WiFi.status() != WL_CONNECTED && attempt < 10) {
    delay(1000);
    Serial.print(".");
    attempt++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWiFi connection failed. Restarting...");
    delay(2000);
    ESP.restart();
  }
}

void saveCredentials(const char* ssid, const char* password) {
  WiFiCredentials credentials;
  memset(credentials.ssid, 0, sizeof(credentials.ssid)); // Clearing memory
  memset(credentials.password, 0, sizeof(credentials.password)); // Clearing memory
  strncpy(credentials.ssid, ssid, sizeof(credentials.ssid) - 1); // Copying SSID
  strncpy(credentials.password, password, sizeof(credentials.password) - 1); // Copying password
  EEPROM.put(0, credentials);
  EEPROM.commit();
}

void readCredentials(char* ssid, char* password) {
  WiFiCredentials credentials;
  EEPROM.get(0, credentials);
  strncpy(ssid, credentials.ssid, sizeof(credentials.ssid)); // Copying SSID
  strncpy(password, credentials.password, sizeof(credentials.password)); // Copying password
}

#include <ESP8266WiFi.h>
#include <EEPROM.h>

#define EEPROM_SIZE 64

#define NUM_READS 10          // Number of sensor readings to average
#define READ_DELAY_MS 1000    // Delay between sensor readings in milliseconds
#define WAIT_DELAY_MS 10000   // Wait time between measurements in milliseconds
#define SLEEP_DURATION_US 600000000  // Deep sleep duration in microseconds (10 minutes)

const char* SERVER = "your.server.address";  // Replace with your server address
const int SERVER_PORT = 80;                  // Replace with your server port

WiFiClient client;  // WiFi client for server connection

void saveCredentials(const char *ssid, const char *password);
void readCredentials(char *ssid, char *password);
void listNetworks();
const boolean deepSleep = true;

void setup() {
  pinMode(D4, INPUT);  // Ensure the D4 pin is set to input for reading sensor
  Serial.begin(9600);
  while (!Serial) {
  }

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
    while (!Serial.available()) {
    }
    String ssid = Serial.readStringUntil('\n');
    ssid.trim();
    Serial.println("Enter WiFi Password:");
    while (!Serial.available()) {
    }
    String password = Serial.readStringUntil('\n');
    password.trim();

    saveCredentials(ssid.c_str(), password.c_str());

    Serial.println("WiFi credentials saved. Rebooting...");
    delay(2000);
    ESP.restart();
  }
}

void loop() {
  Serial.println("Starting soil moisture measurement...");
  int totSum = 0;
  for (int k = 0; k < NUM_READS; k++) {
    int sensorValue = digitalRead(D4);  // Read the digital value from the sensor
    Serial.print("Read sensor value: ");
    Serial.println(sensorValue);
    totSum += sensorValue;
    delay(READ_DELAY_MS);
  }

  int averageValue = totSum / NUM_READS;
  int moisture = map(averageValue, 0, 1, 100, 0); // Mapping the sensor value to 0-100%

  Serial.print("Calculated moisture value: ");
  Serial.println(moisture);

  Serial.print("Attempting to report moisture value of ");
  Serial.print(moisture);
  Serial.print(" to server at: ");
  Serial.print(SERVER);
  Serial.print(":");
  Serial.println(SERVER_PORT);

  if (client.connect(SERVER, SERVER_PORT)) {
    String postStr = "sensorVal=" + String(moisture);

    client.print("POST /saturation HTTP/1.1\r\n");
    client.print("Host: " + String(SERVER) + "\r\n");
    client.print("Connection: close\r\n");
    client.print("Content-Type: application/x-www-form-urlencoded\r\n");
    client.print("Content-Length: " + String(postStr.length()) + "\r\n");
    client.print("\r\n");
    client.print(postStr);

    client.stop();
    Serial.println("Hooray! The post was successfully processed!");
  } else {
    Serial.println("The post could not be processed or timed out.");
  }

  if (deepSleep == false) {
    Serial.println("Waiting before repeating the measurement...");
    delay(WAIT_DELAY_MS);
  } else {
    Serial.println("Dropping to Deep Sleep...");
    ESP.deepSleep(SLEEP_DURATION_US);
  }
}

void saveCredentials(const char *ssid, const char *password) {
  EEPROM.begin(EEPROM_SIZE);
  for (int i = 0; i < 32; ++i) {
    if (ssid[i] == '\0')
      break;
    EEPROM.write(i, ssid[i]);
  }
  for (int i = 0; i < 32; ++i) {
    if (password[i] == '\0')
      break;
    EEPROM.write(32 + i, password[i]);
  }
  EEPROM.commit();
}

void readCredentials(char *ssid, char *password) {
  EEPROM.begin(EEPROM_SIZE);
  for (int i = 0; i < 32; ++i) {
    ssid[i] = EEPROM.read(i);
    if (ssid[i] == '\0')
      break;
  }
  for (int i = 0; i < 32; ++i) {
    password[i] = EEPROM.read(32 + i);
    if (password[i] == '\0')
      break;
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

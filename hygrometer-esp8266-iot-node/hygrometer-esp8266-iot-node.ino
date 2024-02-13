#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>

const char *SERVER = "192.168.86.36";
const int SERVER_PORT = 3000;
const int SLEEP_MS = 5 * 60e6;
const boolean deepSleep = false;

WiFiManager wifiManager;
const char *softAPSSID = "Plant_Module_Setup";
const char *softAPPass = "Bio::Neos";
const char *softAPHostname = "Plant_Module_Setup";
A
const int NUM_READS = 10;
const int READ_DELAY_MS = 1000;
const int WAIT_DELAY_MS = 50000;
const int MAX_SOFTAP_CLIENTS = 1;

WiFiClient client;
WiFiServer server(80); // Declare server

void handleClearCredentials()
{
  Serial.println("Clearing Wi-Fi credentials...");
  wifiManager.resetSettings();
  Serial.println("Wi-Fi credentials cleared. Restarting...");
  ESP.restart();
}

void setup()
{
  Serial.begin(9600);
  delay(10);

  Serial.println("Starting WiFi setup...");

  wifiManager.setAPStaticIPConfig(IPAddress(192, 168, 1, 1), IPAddress(192, 168, 1, 1), IPAddress(255, 255, 255, 0));
  wifiManager.setDebugOutput(false);

  if (!wifiManager.autoConnect(softAPSSID, softAPPass))
  {
    Serial.println("Failed to connect and hit timeout");
    delay(3000);
    ESP.reset();
    delay(5000);
  }

  // Check if connected to WiFi
  if (WiFi.status() == WL_CONNECTED)
  {
    // Configure SOFTAP
    WiFi.softAPConfig(IPAddress(192, 168, 1, 1), IPAddress(192, 168, 1, 1), IPAddress(255, 255, 255, 0));
    WiFi.softAP(softAPSSID, softAPPass);
    // Set the hostname using the DHCP server hostname option
    WiFi.hostname(softAPHostname);

    server.begin(); // Start the server

    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("ESP Board MAC Address: ");
    Serial.println(WiFi.macAddress());

    pinMode(A0, INPUT);

    Serial.println("Setup completed.");
  }
  else
  {
    Serial.println("WiFi connection failed. Check your credentials or connection.");
  }
}

void loop()
{

  if (WiFi.softAPgetStationNum() > MAX_SOFTAP_CLIENTS)
  {
    Serial.println("Max clients reached. Disconnecting additional clients.");
    WiFiClient client = server.available();
    if (client)
    {
      // Disconnect the first connected client
      client.stop();
      Serial.println("Client disconnected.");

      // Additional condition to clear credentials (customize as needed)
      if (someCondition)
      {
        handleClearCredentials();
      }
    }
  }

  Serial.println("Starting soil moisture measurement...");
  // Read 10 values from the sensor, 1 second apart
  int totSum = 0;
  for (int k = 0; k < NUM_READS; k++)
  {
    int sensorValue = analogRead(A0);
    Serial.print("Read sensor value: ");
    Serial.println(sensorValue);
    totSum += sensorValue;
    delay(READ_DELAY_MS);
  }

  // Calculate the moisture value
  int moisture = ((totSum / NUM_READS) / 900) * 100;
  Serial.print("Calculated moisture value: ");
  Serial.println(moisture);

  // Open a basic HTTP connection to the server
  Serial.print("Attempting to report moisture value of ");
  Serial.print(moisture);
  Serial.print(" to server at: ");
  Serial.print(SERVER);
  Serial.print(":");
  Serial.println(SERVER_PORT);

  if (client.connect(SERVER, SERVER_PORT))
  {
    // Create our POST request message Body content
    String postStr = "sensorVal=";
    postStr += String(moisture);

    // Send our POST request
    client.print("POST /saturation HTTP/1.1\r\n");
    client.print("Host: localhost\r\n");
    client.print("Connection: close\r\n");
    client.print("Content-Type: application/x-www-form-urlencoded\r\n");
    client.print("Content-Length: " + String(postStr.length()) + "\r\n");
    client.print("\r\n");
    client.print(postStr);

    // Close our HTTP connection
    client.stop();
    Serial.println("Hooray! The post was successfully processed!");
  }
  else
  {
    Serial.println("The post could not be processed or timed out.");
  }

  // Wait in Deep Sleep before repeating the measurement (to save battery)
  // SEE ALSO: https://randomnerdtutorials.com/esp8266-deep-sleep-with-arduino-ide/
  // NOTE: disable this delay to enable the module to send sensor values every 10 seconds

  if (deepSleep == false)
  {
    Serial.println("Waiting before repeating the measurement...");
    delay(WAIT_DELAY_MS);
  }
  else
  {
    Serial.println("Dropping to Deep Sleep...");
    ESP.deepSleep(SLEEP_MS);
  }
}

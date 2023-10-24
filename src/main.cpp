#include <Arduino.h>
#include <RoombaCommunication.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// WiFi setup
const char* ssid = "ssid";
const char* password = "password";
// MQTT setup
const char* mqtt_server = "mqtt_server";
const char* mqttUser = "mqttUser";
const char* mqttPassword = "mqttPassword";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;
const int capacity=JSON_OBJECT_SIZE(3)+2*JSON_OBJECT_SIZE(1);
StaticJsonDocument<capacity> JSONpayload;

#define SERIAL_RX     D5  // pin for SoftwareSerial RX
#define SERIAL_TX     D6  // pin for SoftwareSerial TX
#define WAKE_PIN      D1
long baudRate = 115200;
RoombaCommunication roomba(SERIAL_RX, SERIAL_TX, WAKE_PIN, baudRate);

int DEFAULT_SPEED = 50;
//--------------------------------------------------------------------------

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  DeserializationError error = deserializeJson(JSONpayload, payload);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
  }
  else if (strcmp(topic,"/homeassistant/roomba/wakeup")==0) {
    roomba.wakeUp();
  }
  else if (strcmp(topic,"/homeassistant/roomba/dock")==0) {
    roomba.seekDock();
  }
  else if (strcmp(topic,"/homeassistant/roomba/rotate")==0) {
    int input_speed = JSONpayload["data"];
    Serial.println("Data:");
    Serial.println(input_speed);
    roomba.driveDirect(input_speed, -input_speed);
  }
  else if (strcmp(topic,"/homeassistant/roomba/forward")==0) {
    int input_speed = JSONpayload["data"];
    Serial.println("Data:");
    Serial.println(input_speed);
    roomba.driveDirect(input_speed, input_speed);
  }
  else if (strcmp(topic,"/homeassistant/roomba/backward")==0) {
    int input_speed = JSONpayload["data"];
    Serial.println("Data:");
    Serial.println(input_speed);
    roomba.driveDirect(-input_speed, -input_speed);
  }
  else if (strcmp(topic,"/homeassistant/roomba/stop")==0) {
    roomba.driveStop();
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(),mqttUser,mqttPassword)) {
      Serial.println("connected");
      // ... and resubscribe
      client.subscribe("/homeassistant/roomba/+");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

//--------------------------------------------------------------------------

void setup()
{ 
  Serial.begin(115200);
  pinMode(WAKE_PIN, OUTPUT);
  pinMode(SERIAL_RX, INPUT);
  pinMode(SERIAL_TX, OUTPUT);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop()
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  delay(1);
}
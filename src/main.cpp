#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <RoombaCommunication.h>


const char *ssid = "RobotControllerAP";
const char *password = "password";

ESP8266WebServer server(80);

#define SERIAL_RX     D5  // pin for SoftwareSerial RX
#define SERIAL_TX     D6  // pin for SoftwareSerial TX
#define WAKE_PIN      D1
long baudRate = 115200;
RoombaCommunication roomba(SERIAL_RX, SERIAL_TX, WAKE_PIN, baudRate);

int DEFAULT_SPEED = 50;

void handleRoot() {
  server.send(200, "text/html", "<html><body>"
                                "<h1>Robot Control</h1>"
                                "<form action='/move' method='get'>"
                                "<button name='direction' value='forward'>Forward</button>"
                                "<button name='direction' value='backward'>Backward</button>"
                                "<button name='direction' value='left'>Left</button>"
                                "<button name='direction' value='right'>Right</button>"
                                "<button name='command' value='wakeup'>Wake Up</button>"
                                "<button name='command' value='dock'>Dock</button>"
                                "</form>"
                                "</body></html>");
}

void handleMove() {
  String direction = server.arg("direction");
  String command = server.arg("command");

  if (direction == "forward") {
    Serial.println("Moving Forward");
    roomba.driveDirect(DEFAULT_SPEED, DEFAULT_SPEED);
  } else if (direction == "backward") {
    Serial.println("Moving Backward");
    roomba.driveDirect(-DEFAULT_SPEED, -DEFAULT_SPEED);
  } else if (direction == "left") {
    Serial.println("Moving Left");
    roomba.driveDirect(-DEFAULT_SPEED, DEFAULT_SPEED);
  } else if (direction == "right") {
    Serial.println("Moving right");
    roomba.driveDirect(DEFAULT_SPEED, -DEFAULT_SPEED);
  } else if (command == "wakeup") {
    Serial.println("Moving wakeup");
    roomba.wakeUp();
  } else if (command == "dock") {
    Serial.println("Moving dock");
    roomba.seekDock();
  }

  server.send(200, "text/plain", "OK");
}


void setup() {
  Serial.begin(115200);

  // Set up Access Point
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  
  // Print the IP address
  Serial.println();
  Serial.print("Access Point IP Address: ");
  Serial.println(WiFi.softAPIP());

  // Serve the web page
  server.on("/", HTTP_GET, handleRoot);
  server.on("/move", HTTP_GET, handleMove);
  
  server.begin();
}

void loop() {
  server.handleClient();
}

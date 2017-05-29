#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#define STREET_DOOR 5
#define GARAGE_DOOR 4
//CHANGE TO YOUR Wi-Fi NETWORK
const char* ssid = "Bruno's Office Wi-Fi Network";
//SET YOUR NETWORK Wi-Fi PASSWORD
const char* password = "ARDUINO2016";


ESP8266WebServer server(80);

void handleExternalDoor() {
  digitalWrite(STREET_DOOR, HIGH);
  delay(2000);
  digitalWrite(STREET_DOOR, LOW);
  server.send(200, "text/plain","1");
}

void handleGarageDoor() {
  digitalWrite(GARAGE_DOOR, HIGH);
  delay(2000);
  digitalWrite(GARAGE_DOOR, LOW);
  server.send(200, "text/plain","0");
}
void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  
}

void setup(){
  //SETUP PINS
  pinMode(EXTERNAL_DOOR, OUTPUT);
  pinMode(GARAGE_DOOR, OUTPUT);
  
  //INIT SERIAL FOR DEBUG
  Serial.begin(115200);

  //START WI-FI CONNECTIONS
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  //INIT ZERO CONF
  if (MDNS.begin("door_controller")) {
    Serial.println("MDNS responder started");
  }

  //REGISTER END POINTS
  server.on("/external-door", handleExternalDoor);
  server.on("/garage-door", handleGarageDoor);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient();
}

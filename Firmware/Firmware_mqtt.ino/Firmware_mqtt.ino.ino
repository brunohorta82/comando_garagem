#include <MQTT.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>;
#define BUFFER_SIZE 100
#define STREET_DOOR 5
#define GARAGE_DOOR 4
const char *ssid =  "Bruno's Wi-Fi Network";    // cannot be longer than 32 characters!
const char *pass =  "ARDUINO2016";    //

// Update these with values suitable for your network.
IPAddress server(213, 136, 83, 118);
//IPAddress server(192, 168,187,70);

String garagedoorState = "LOCK";
String streetdoorState = "LOCK";

WiFiClient wclient;
PubSubClient client(wclient, server);



void handleStreetDoor() {
  digitalWrite(STREET_DOOR, HIGH);
  delay(2000);
  digitalWrite(STREET_DOOR, LOW);
  Serial.println("Street");
  
}

void handleGarageDoor() {
  digitalWrite(GARAGE_DOOR, HIGH);
  delay(2000);
  digitalWrite(GARAGE_DOOR, LOW);
Serial.println("Garage");
}
void callback(const MQTT::Publish&pub) {
  // handle message arrived
  Serial.print(pub.topic());
  Serial.print(" => ");
    String payload = pub.payload_string();
    Serial.println(payload);
    if(pub.topic() == "home-assistant/garagedoor/set"){
          if(payload != garagedoorState){
            garagedoorState= payload;
            handleGarageDoor();
          }
      }else if (pub.topic() == "home-assistant/streetdoor/set"){
        
          if(payload != streetdoorState){
            streetdoorState = payload;
            handleStreetDoor();
      }
       
      }
        client.publish("home-assistant/streetdoor",streetdoorState);
        client.publish("home-assistant/garagedoor",garagedoorState);
}    
  
void setup() {
  //Ativar apenas modo estaÃ§Ã£o
  WiFi.mode(WIFI_STA);
pinMode(GARAGE_DOOR,OUTPUT);
pinMode(STREET_DOOR,OUTPUT);
  // Setup console
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println();
  client.set_callback(callback);
  
}

void loop() {

  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to ");
    Serial.print(ssid);
    Serial.println("...");
    WiFi.begin(ssid, pass);

    if (WiFi.waitForConnectResult() != WL_CONNECTED)
      return;
    Serial.println("WiFi connected");
  }

  if (WiFi.status() == WL_CONNECTED) {
    if (!client.connected()) {
      if (client.connect("GarageDoorController")) {
        client.subscribe("home-assistant/garagedoor/set");
        client.subscribe("home-assistant/streetdoor/set");
        client.publish("home-assistant/streetdoor",streetdoorState);
        client.publish("home-assistant/garagedoor",garagedoorState);
           
      }
    }

    if (client.connected()){
      client.loop();
    }

}
}


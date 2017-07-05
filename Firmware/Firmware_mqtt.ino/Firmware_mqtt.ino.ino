#include <PubSubClient.h>
//Wi-Fi Manger library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>//https://github.com/tzapu/WiFiManager
#define STREET_DOOR 5
#define GARAGE_DOOR 4
#define BUFFER_SIZE 100
#define STREET_DOOR_BUSY_TIME  21000
#define GARAGE_DOOR_BUSY_TIME 21000

// Update these with values suitable for your network.
IPAddress server(192, 168,187,203);

String garagedoorState = "UNDEFINED";
String streetdoorState = "UNDEFINED";

int delayGarageDoorTimeToOpenClose = 0;
int delayStreetDoorTimeToOpenClose = 0;

WiFiClient wclient;
PubSubClient client(wclient, server);
void setup() {
  // Setup console
  Serial.begin(115200);
  WiFiManager wifiManager;
  //reset saved settings
   // wifiManager.resetSettings();
  wifiManager.autoConnect("GarageDoorControl");
  pinMode(GARAGE_DOOR,OUTPUT);
  pinMode(STREET_DOOR,OUTPUT);
  client.set_callback(callback);
}



void handleStreetDoor(String action) {
  if(garagedoorState == "UNDEFINED" || delayStreetDoorTimeToOpenClose > millis()){
    return; 
   }
   
   if(streetdoorState != action){
    streetdoorState = action;
    digitalWrite(STREET_DOOR, HIGH);
    delay(2000);
    digitalWrite(STREET_DOOR, LOW);
    delayStreetDoorTimeToOpenClose = millis() + STREET_DOOR_BUSY_TIME;
   }
}

void handleGarageDoor(String action) {
  if(delayGarageDoorTimeToOpenClose > millis()){
    Serial.println("Busy");
    Serial.println(garagedoorState);
    }else{
      Serial.println("ready");
      Serial.println(garagedoorState);
      }
  if(garagedoorState == "UNDEFINED" || delayGarageDoorTimeToOpenClose > millis()){
    return; 
   }
   
   if(garagedoorState != action){
    garagedoorState = action;
    
    digitalWrite(GARAGE_DOOR, HIGH);
    delay(2000);
    digitalWrite(GARAGE_DOOR, LOW);
    delayGarageDoorTimeToOpenClose = millis() + GARAGE_DOOR_BUSY_TIME;
   }
}

void callback(const MQTT::Publish&pub) {
    String payload = pub.payload_string();
    if(pub.topic() == "home-assistant/garagedoor/set"){
          if(payload != garagedoorState){
            handleGarageDoor(payload);
          }
      }else if (pub.topic() == "home-assistant/streetdoor/set"){
          if(payload != streetdoorState){
            handleStreetDoor(payload);
      }
      }else if (pub.topic() == "home-assistant/garagedoor"){
        garagedoorState = payload;
       
       }       
}    


void loop() {

  if (WiFi.status() == WL_CONNECTED) {
    if (!client.connected()) {
      if (client.connect(MQTT::Connect("GarageDoorControll")
      .set_auth("moscas", "moscasMoscas82"))) {
         Serial.println("Connected to MQTT server");
        client.subscribe("home-assistant/garagedoor/set");
        client.subscribe("home-assistant/streetdoor/set");
        client.subscribe("home-assistant/garagedoor");  
        client.subscribe("home-assistant/streetdoor");   
      }else{
        Serial.println("MQTT ERROR!");
        }
    }

    if (client.connected()){
      client.loop();
    }

}
  
}


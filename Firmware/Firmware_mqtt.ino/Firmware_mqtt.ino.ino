#include <MQTT.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>;
#define BUFFER_SIZE 100
#define STREET_DOOR 5
#define GARAGE_DOOR 4
const char *ssid =  "Bruno's Wi-Fi Network";    // cannot be longer than 32 characters!
const char *pass =  "PW";    //

// Update these with values suitable for your network.

IPAddress server(192, 168,187,70);

String garagedoorState = "UNDEFINED";
String streetdoorState = "UNDEFINED";

WiFiClient wclient;
PubSubClient client(wclient, server);



void handleStreetDoor(String action) {
   if(streetdoorState == "UNDEFINED"){
    streetdoorState = action;
    Serial.println("Não posso fazer nada, o estado do portão da rua ainda é indefenido");
    return; 
   }
  digitalWrite(STREET_DOOR, HIGH);
  delay(2000);
  digitalWrite(STREET_DOOR, LOW);
  Serial.println("Street");
  
}

void handleGarageDoor(String action) {
  if(garagedoorState == "UNDEFINED"){
    Serial.println("Não posso fazer nada, o estado do portão da garagem ainda é indefenido");
    return; 
   }
   if(garagedoorState != action){
    Serial.print("ACTION ");
    Serial.println(action);
    garagedoorState = action;
    digitalWrite(GARAGE_DOOR, HIGH);
    delay(2000);
    digitalWrite(GARAGE_DOOR, LOW);
     Serial.println("Garage");
   }
}
void callback(const MQTT::Publish&pub) {
  // handle message arrived
  Serial.print(pub.topic());
  Serial.print(" => ");
    String payload = pub.payload_string();
    Serial.println(payload);
    if(pub.topic() == "home-assistant/garagedoor/set"){
          if(payload != garagedoorState){
            handleGarageDoor(payload);
          }
      }else if (pub.topic() == "home-assistant/streetdoor/set"){
          if(payload != streetdoorState){

            handleStreetDoor(payload);
             client.publish("home-assistant/streetdoor",streetdoorState);

      }
      }else if (pub.topic() == "home-assistant/garagedoor"){
        garagedoorState = payload;
        Serial.print("Recebi o Estado ");
        Serial.println(payload);
       }
       
      
        
}    
  
void setup() {
  //Ativar apenas modo estaÃ§Ã£o
  WiFi.mode(WIFI_STA);
  pinMode(GARAGE_DOOR,OUTPUT);
  pinMode(STREET_DOOR,OUTPUT);
  // Setup console
  Serial.begin(115200);
  delay(10);
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
        client.subscribe("home-assistant/garagedoor");   
      }
    }

    if (client.connected()){
      client.loop();
    }

}
}


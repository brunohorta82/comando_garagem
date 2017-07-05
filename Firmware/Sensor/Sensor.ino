#include <PubSubClient.h>
//Wi-Fi Manger library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>//https://github.com/tzapu/WiFiManager
#define BUFFER_SIZE 100
#define SENSOR_CLOSE 4

// Update these with values suitable for your network.
IPAddress server(192, 168,187,203);

String notificationState = "UNDEFINED";
long lastNotifTime = 0;
long notifDelay = 2000;
WiFiClient wclient;
PubSubClient client(wclient, server);

void setup() {
  // Setup console
  Serial.begin(115200);
  WiFiManager wifiManager;
  //reset saved settings
//wifiManager.resetSettings();
  wifiManager.autoConnect("GarageDoorSensor");
  pinMode(SENSOR_CLOSE,INPUT);
 
}
String realState = "UNDEFINED";
void loop() {


  if (WiFi.status() == WL_CONNECTED) {
    if (!client.connected()) {
      if (client.connect(MQTT::Connect("GarageDoorSensor")
      .set_auth("moscas", "moscasMoscas82"))) {
        Serial.println("Connected to MQTT server");
        if(realState != "UNDEFINED"){
          client.publish_P("home-assistant/garagedoor",realState.c_str(),realState.length(),true);
        }
      }else{
        Serial.println("MQTT ERROR!");
        }
    }

    if (client.connected()){
      client.loop();
    }
  
  //CORRECT WAY TO ROLLOVER millis() FUNCTION
  unsigned long currentMillis = millis();
  if ((unsigned long)(currentMillis - lastNotifTime) >= notifDelay) {
    int sensorVal = digitalRead(SENSOR_CLOSE);
    lastNotifTime = currentMillis;
    if(sensorVal){
      realState = "LOCK";
    }else{
      realState = "UNLOCK";     
    }
    if(realState != notificationState){
      client.publish_P("home-assistant/garagedoor",realState.c_str(),realState.length(),true);
      notificationState = realState;
      }
  }
 
}
}


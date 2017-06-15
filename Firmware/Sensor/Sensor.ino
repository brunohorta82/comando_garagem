#include <MQTT.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>;
#define BUFFER_SIZE 100
#define SENSOR_CLOSE 4


const char *ssid =  "Bruno's Wi-Fi Network";    // cannot be longer than 32 characters!
const char *pass =  "PW";    //

// Update these with values suitable for your network.

IPAddress server(192, 168,187,70);

String notificationState = "UNDEFINED";
String readState = "UNDEFINED";  
long lastNotifTime = 0;
long notifDelay = 2000;
WiFiClient wclient;
PubSubClient client(wclient, server);

void setup() {
  //Ativar apenas modo estaÃ§Ã£o
  WiFi.mode(WIFI_STA);
  pinMode(SENSOR_CLOSE,INPUT);
  // Setup console
  Serial.begin(115200);
  delay(10);
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
      if (client.connect(MQTT::Connect("GarageDoorSensor")
      .set_auth("moscas", "moscasMoscas82"))) {
        Serial.println("Connected to MQTT server");
      }else{
        Serial.println("MQTT ERROR!");
        }
    }

    if (client.connected()){
      client.loop();
    }
    //FAIL SAFE TO RESOLVE LIMITATION Of millis() funtion
  if(millis() < lastNotifTime){
    lastNotifTime = 0;
    }
  if ((millis() - lastNotifTime) > notifDelay) {
      int sensorVal = digitalRead(SENSOR_CLOSE);
    lastNotifTime = millis();
    if(sensorVal){
       client.publish("home-assistant/garagedoor","LOCK");
    }else{
     client.publish("home-assistant/garagedoor","UNLOCK");
    }
  
  }
 
}
}


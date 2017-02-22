#include <ESP8266WiFi.h>          

#include <DNSServer.h>            
#include <ESP8266WebServer.h>     
#include <WiFiManager.h>

#include <PubSubClient.h>

WiFiClient wifiClient;
PubSubClient client(wifiClient);
WiFiManager wifiManager;

long lastMsg = 0;
char msg[50];
int value = 0;

const char* mqtt_server = "192.168.1.7";

const int BUTTON = 4;
const int GREEN = 12;

// WIFI PART //
void setup_wifi() {
  wifiManager.autoConnect();
}
// WIFI PART //


// MQTT PART //
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void setup_mqtt() {
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}
// MQTT PART //

void setup_gpio() {
  pinMode(BUTTON, INPUT);
  pinMode(GREEN, OUTPUT);
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  setup_mqtt();
  setup_gpio();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  int buttonValue = digitalRead(BUTTON);

  if ( buttonValue > 0 ) {
    digitalWrite(GREEN, LOW);
  } else {
    digitalWrite(GREEN, HIGH);
    snprintf (msg, 75, "Button pushed");
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("outTopic", msg);
    delay(300);
    digitalWrite(GREEN, LOW);
    delay(5000);
  } 
}

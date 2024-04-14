#include <PubSubClient.h>
#include <WiFiNINA.h>
#include "secret.h"

// MQTT Broker settings
const char* mqtt_broker = "mqtt-dashboard.com";
const char* topic = "SIT210/wave";
const int mqtt_port = 1883;

// Ultrasonic Sensor Pins
const int trigPin = 7;
const int echoPin = 8;

// LED Pin
const int ledPin = 9;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(9600);
  while (!Serial) {}
  connectWifi();
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long duration, distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;

  if (distance < 30) {
    client.publish(topic, "Pat detected!");
  }

  if (distance >= 30 && distance < 100) {
    client.publish(topic, "Wave detected!");
  }

  delay(2000); // Delay between measurements
}

void connectWifi() {
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(SECRET_SSID, SECRET_PASS);
      Serial.print(".");
      delay(5000);     
    }
  }
  Serial.println("\nWiFi connected!");
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received: ");
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  // Determine action based on message content
  if (message.indexOf("Wave") > -1) {
    // Flash LED 3 times quickly
    for (int i = 0; i < 3; i++) {
      digitalWrite(ledPin, HIGH);
      delay(100);
      digitalWrite(ledPin, LOW);
      delay(100);
    }
  } else if (message.indexOf("Pat") > -1) {
    // Flash LED 2 times slowly
    for (int i = 0; i < 2; i++) {
      digitalWrite(ledPin, HIGH);
      delay(500);
      digitalWrite(ledPin, LOW);
      delay(500);
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    if (client.connect("Arduino33IoT")) {
      Serial.println("Connected!");
      client.subscribe(topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>

#include <SPI.h>
#include <LoRa.h>

#include "defines.h"

void mqttDataCallback(char* topic, byte* payload, unsigned int length);
void mqttDisconnectedCallback();

boolean pendingDisconnect = true;

WiFiClient wclient;
PubSubClient client(MQTT_HOST, MQTT_PORT, mqttDataCallback, wclient);

void mqttDataCallback(char* topic, byte* payload, unsigned int length) {
  if(DEBUG_PRINT_SERIAL) {
    Serial.println("MQTT Message Received!");
    Serial.print("\tTopic: \t");
    Serial.println(topic);
    Serial.print("\tMessage: \t");
    
    payload[length] = '\0';
    String s = String((char*)payload);
    Serial.println(s);
  }

  String strTopic = String(topic);
  if (strTopic.equals(MQTT_STATUS_REQUEST_TOPIC)) {
      String response = String("IP:");
      response += WiFi.localIP().toString();
      if(DEBUG_PRINT_SERIAL) {
        Serial.println("Publish Technical Status Report");
        Serial.print("\tTopic: \t");
        Serial.println(MQTT_STATUS_RESPONSE_TOPIC);
        Serial.print("\tMessage: \t");
        Serial.println(response);
      }
      client.publish(MQTT_STATUS_RESPONSE_TOPIC, response.c_str());
  }
}

void mqttConnectedCallback() {
  if(DEBUG_PRINT_SERIAL) {
    Serial.println("MQTT Connected");
  }

  if(DEBUG_PRINT_SERIAL) {
    Serial.println("Subscribing to status messages");
    Serial.print("\tTopic:\t");
    Serial.println(MQTT_STATUS_REQUEST_TOPIC);
  }
  client.subscribe(MQTT_STATUS_REQUEST_TOPIC);

  if(DEBUG_PRINT_SERIAL) {
    Serial.println("Publishing Connected message");
    Serial.print("\tTopic:\t");
    Serial.println(MQTT_CONNECTED_TOPIC);
    Serial.print("\tMessage:\t");
    Serial.println(MQTT_CONNECTED_MESSAGE);
  }
  client.publish(MQTT_CONNECTED_TOPIC, MQTT_CONNECTED_MESSAGE);
}

void mqttDisconnectedCallback() {
  if(DEBUG_PRINT_SERIAL) {
    Serial.println("MQTT Disconnected");
  }
}

void setup() {
  if(DEBUG_PRINT_SERIAL) {
    Serial.begin(SERIAL_BAUDRATE);
    Serial.println("");
    Serial.println("Setup...");
  }

  setupWifi();

  setupLoRa();

  if(DEBUG_PRINT_SERIAL) {
    Serial.println("Setup Completed");
  }
}

void setupWifi() {
  ArduinoOTA.setHostname(WIFI_HOSTNAME);

  if(DEBUG_PRINT_SERIAL) {
    Serial.println("Connecting to WiFi");
    Serial.print("\tSSID:\t");
    Serial.println(WIFI_SSID);
    Serial.print("\tHostname:\t");
    Serial.println(WIFI_HOSTNAME);
  }
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    if(DEBUG_PRINT_SERIAL) {
      Serial.print(".");
    }
    delay(500);
  }
  
  if(DEBUG_PRINT_SERIAL) {
    Serial.println("\nWiFi Connected");
    Serial.print("\tIP address:\t");
    Serial.println(WiFi.localIP());
  }
}

void setupLoRa() {
  LoRa.setPins(SS_PIN, RESET_PIN, IRQ_PIN);// set CS, reset, IRQ pin
  
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  LoRa.onReceive(onReceive);
  LoRa.receive();

  if(DEBUG_PRINT_SERIAL) {
    Serial.println("\nLoRa Connected");
    Serial.print("\tRSSI:\t");
    Serial.println(LoRa.rssi());
  }
}

void processNet() {
  if (WiFi.status() == WL_CONNECTED) {
    ArduinoOTA.begin();
    ArduinoOTA.handle();
    if (client.connected()) {
      client.loop();
    } else {
      if(DEBUG_PRINT_SERIAL) {
        Serial.print("Connecting to MQTT\n\tHost:\t");
        Serial.println(MQTT_HOST);
        Serial.print("\tPort:\t");
        Serial.println(MQTT_PORT);
        Serial.print("\tClient ID:\t");
        Serial.println(MQTT_CLIENT_ID);
        Serial.print("\tUsername:\t");
        Serial.println(MQTT_USERNAME);
        Serial.print("\tLast Will Topic:\t");
        Serial.println(MQTT_LAST_WILL_TOPIC);
        Serial.print("\tLast Will Message:\t");
        Serial.println(MQTT_LAST_WILL_MESSAGE);
      }
      
      if (client.connect(MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD, MQTT_LAST_WILL_TOPIC, 1, 0, MQTT_LAST_WILL_MESSAGE)) {
          pendingDisconnect = false;
          mqttConnectedCallback();
      }
    }
  } else {
    if (client.connected())
      client.disconnect();
  }
  if (!client.connected() && !pendingDisconnect) {
    pendingDisconnect = true;
    mqttDisconnectedCallback();
  }
}

void loop() {
  processNet();
}

void onReceive(int packetSize) {
  if(DEBUG_PRINT_SERIAL) {
    Serial.println("LoRa Message Received!");
    Serial.print("\tPacket size:\t");
    Serial.println(packetSize);
  }

  String payload = "";
  for (int i = 0; i < packetSize; i++) {
    payload += (char)LoRa.read();
  }

  if(DEBUG_PRINT_SERIAL) {
    Serial.print("\tPayload:\t");
    Serial.println(payload);

    Serial.print("\tRSSI:\t");
    Serial.println(LoRa.packetRssi());
    Serial.print("\tSNR: \t");
    Serial.println(LoRa.packetSnr());
    Serial.print("Will publish to MQTT topic:\t");
    Serial.println(MQTT_SEND_TOPIC);
  }

  client.publish(MQTT_SEND_TOPIC, payload.c_str());
}

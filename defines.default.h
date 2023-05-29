// WiFi
#define WIFI_SSID "***"
#define WIFI_PASSWORD "***"
#define WIFI_HOSTNAME "LoRa2MQTT"

// MQTT
#define MQTT_HOST "1.2.3.4"
#define MQTT_PORT 1883
#define MQTT_CLIENT_ID "LoRa2MQTT"
#define MQTT_USERNAME "***"
#define MQTT_PASSWORD "***"
#define MQTT_CONNECTED_TOPIC "LoRa2MQTT/Connected"
#define MQTT_CONNECTED_MESSAGE "true"
#define MQTT_LAST_WILL_TOPIC "LoRa2MQTT/Connected"
#define MQTT_LAST_WILL_MESSAGE "false"
#define MQTT_STATUS_REQUEST_TOPIC "LoRa2MQTT/TechnicalStatusRequest"
#define MQTT_STATUS_RESPONSE_TOPIC "LoRa2MQTT/TechnicalStatus"
#define MQTT_SEND_TOPIC "LoRa2MQTT/Message"
#define MQTT_RECEIVE_TOPIC "LoRa2MQTT/Out"
#define MQTT_DEBUG_ENABLED true
#define MQTT_DEBUG_TOPIC "LoRa2MQTT/Debug"

// LoRa
#define SS_PIN D8 // Slave Select pin
#define RESET_PIN D2 // Reset pin
#define IRQ_PIN D1 // Interrupt pin

// Debug
#define SERIAL_BAUDRATE 115200
#define DEBUG_PRINT_SERIAL true

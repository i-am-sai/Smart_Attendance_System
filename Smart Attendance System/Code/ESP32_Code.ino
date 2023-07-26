#include <WiFi.h>
#include <PubSubClient.h>


#define RXp2 16
#define TXp2 17

const char* WIFI_SSID = "Wifi_Name";
const char* WIFI_PASSWORD = "Password";
const char* TOKEN = "ID";
const char* THINGSBOARD_SERVER = "demo.thingsboard.io";

WiFiClient wifiClient;
PubSubClient client(wifiClient);

unsigned long lastSend = 0;
String userName = ""; // Declare a global variable to store the user name

void setup() {
  Serial.begin(9600);
  
  delay(10);
  Serial2.begin(9600, SERIAL_8N1, RXp2, TXp2);
  InitWiFi();
  client.setServer(THINGSBOARD_SERVER, 1883);
  lastSend = 0;
}

void Send_to_Thingsboard() {
  if (!client.connected()) {
    reconnect();
  }

  if (Serial2.available()) {
    String receivedData = Serial2.readString();
    receivedData.trim();
    Serial.print("Received Data: ");
    Serial.println(receivedData);

    Serial.println("Collecting temperature data.");
  
    String payload = "{";
    payload += "\"Name\":\"" + receivedData + "\""; // Include the received data in the payload
    payload += "}";
  
    char attributes[100];
    payload.toCharArray(attributes, 100);
    client.publish("v1/devices/me/telemetry", attributes);
    Serial.println("Data published to ThingsBoard.");
  }
}

void InitWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32 Device", TOKEN, NULL)) {
      Serial.println("Connected to ThingsBoard");
    } else {
      Serial.print("Failed to connect [");
      Serial.print(client.state());
      Serial.println("] Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void loop() {
  unsigned long currentMillis = millis();
  unsigned long sendInterval = 5000; // Adjust this value as needed

  if (currentMillis - lastSend >= sendInterval) {
    Send_to_Thingsboard();
    lastSend = currentMillis;
  }

  client.loop();
}

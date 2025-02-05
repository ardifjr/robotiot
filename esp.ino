#include <PubSubClient.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <SoftwareSerial.h>

// Konfigurasi WiFi
const char* ssid = "vivo Y02"; // Untuk HTTP
const char* pass = "12345678";

// Konfigurasi broker MQTT
const char* mqtt_server = "broker.hivemq.com"; // Alamat broker
const int mqtt_port = 1883;
const char* mqtt_client_id = "ESP32_Client";
const char* topic = "robotngobrol";

#define TRIG_PIN 18
#define ECHO_PIN 5
#define SOUND_SPEED 0.034

WiFiClient espClient;
PubSubClient mqttClient(espClient);
WiFiClient httpClient;
HTTPClient http;

SoftwareSerial SUART(D5, D6);

String serverName = "http://192.168.61.7:5000";

unsigned long startTime = 0;
unsigned long lastTime = 0;
unsigned long timerDelay = 5000; // Set timer to 5 seconds (5000)
bool isClose = false;

void setup_wifi_mqtt() {
  delay(10);
  Serial.println("Menghubungkan ke WiFi untuk MQTT...");
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nTerhubung ke WiFi untuk MQTT");
  Serial.print("Alamat IP: ");
  Serial.println(WiFi.localIP());
}

void setup_wifi_http() {
  delay(10);
  Serial.println("Menghubungkan ke WiFi untuk HTTP...");
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nTerhubung ke WiFi untuk HTTP");
  Serial.print("Alamat IP: ");
  Serial.println(WiFi.localIP());
}

void reconnect_mqtt() {
  while (!mqttClient.connected()) {
    Serial.print("Menghubungkan ke MQTT...");
    if (mqttClient.connect(mqtt_client_id)) {
      Serial.println("Terhubung ke MQTT");
    } else {
      Serial.print("Gagal, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" coba lagi dalam 5 detik");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  SUART.begin(9600);

  // Setup WiFi dan MQTT
  setup_wifi_mqtt();
  mqttClient.setServer(mqtt_server, mqtt_port);

  // Setup WiFi untuk HTTP
  setup_wifi_http();

  // Setup pin ultrasonic
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

void loop() {
  // MQTT Loop
  if (!mqttClient.connected()) {
    reconnect_mqtt();
  }
  mqttClient.loop();

  // Ultrasonic sensor untuk MQTT
  long duration;
  float distance;

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH);
  distance = (duration * SOUND_SPEED) / 2;

  Serial.print("Jarak: ");
  Serial.print(distance);
  Serial.println(" cm");

  unsigned long currentTime = millis();

  if (distance < 100) {
    if (!isClose) {
      startTime = currentTime;
      isClose = true;
    }

    if (currentTime - startTime > 5000) {
      mqttClient.publish(topic, "Ada orang ngobrol dengan robot");
      Serial.println("Pesan: Ada orang ngobrol dengan robot");
    } else {
      mqttClient.publish(topic, "Ada orang di depan robot");
      Serial.println("Pesan: Ada orang di depan robot");
    }
  } else {
    if (isClose) {
      isClose = false;
    }
    mqttClient.publish(topic, "Ada orang lewat");
    Serial.println("Pesan: Ada orang lewat");
  }

  // HTTP Loop
  if ((millis() - lastTime) > timerDelay) {
    if (WiFi.status() == WL_CONNECTED) {
      String serverPath = serverName + "/poll";

      http.begin(httpClient, serverPath.c_str());

      int httpResponseCode = http.GET();

      if (httpResponseCode > 0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        if (payload != "NO_QUESTION") {
          Serial.println(payload);
          SUART.println(payload);
        }
      } else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      http.end();
    } else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }

  delay(500); // Delay untuk MQTT dan HTTP
}
#include <esp_now.h>
#include <WiFi.h>

#define LED_PIN 2     // Onboard LED (GPIO2)
#define THRESHOLD 30  // Distance threshold in cm

// struct definition
typedef struct __attribute__((packed)) struct_BSM {
  char type[4];
  float distance;
  int speed;
  float acceleration;
  uint32_t timestamp;
} struct_BSM;

volatile float currentDistance = 100.0; // Default safe distance
unsigned long previousMillis = 0;
unsigned long lastDataTime = 0;
const long BLINK_INTERVAL = 250; // Blink interval (ms)
const long DATA_TIMEOUT = 1000; // Timeout (ms)

void OnDataRecv(const uint8_t *mac, const uint8_t *data, int len) {
  struct_BSM receivedData;
  memcpy(&receivedData, data, sizeof(receivedData));
  lastDataTime = millis();
  currentDistance = receivedData.distance;

  // Print all received fields
  Serial.print("Type: ");
  Serial.print(receivedData.type);
  Serial.print(" | Distance: ");
  Serial.print(receivedData.distance);
  Serial.print("cm | Speed: ");
  Serial.print(receivedData.speed);
  Serial.print(" | Acceleration: ");
  Serial.print(receivedData.acceleration);
  Serial.print(" | Timestamp: ");
  Serial.println(receivedData.timestamp);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  WiFi.mode(WIFI_STA);
  
  if (esp_now_init() != ESP_OK) return;
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  // Blink control without blocking
  unsigned long currentMillis = millis();
  
  // Stop everything if data is stale
  if (currentMillis - lastDataTime > DATA_TIMEOUT) {
    digitalWrite(LED_PIN, LOW);
    return;
  }

  // Blink LED if obstacle detected
  if (currentDistance < THRESHOLD) {
    if (currentMillis - previousMillis >= BLINK_INTERVAL) {
      previousMillis = currentMillis;
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    }
  } else {
    digitalWrite(LED_PIN, LOW); // Turn off LED
  }
}
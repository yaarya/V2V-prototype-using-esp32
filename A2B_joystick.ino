#include <esp_now.h>
#include <WiFi.h>
#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE
#include <DabbleESP32.h>

// CONFIGURATION (change these for each device)
const char* DEVICE_NAME = "ESP32-A";  
uint8_t peerMac[] = {0x14, 0x33, 0x5C, 0x30, 0x85, 0x98};

typedef struct __attribute__((packed)) {
  int8_t x;
  int8_t y;
} JoystickData;

JoystickData sentData;
JoystickData receivedData;
int8_t lastX = 0, lastY = 0;

void OnDataRecv(const uint8_t *mac, const uint8_t *data, int len) {
  memcpy(&receivedData, data, sizeof(receivedData));
  Serial.print("Recv X:");
  Serial.print(receivedData.x);
  Serial.print(" Y:");
  Serial.println(receivedData.y);
}

void setup() {
  Serial.begin(115200);
  Dabble.begin(DEVICE_NAME);
  WiFi.mode(WIFI_STA);
  
  if (esp_now_init() != ESP_OK) return;
  
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, peerMac, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  esp_now_add_peer(&peerInfo);
  
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  Dabble.processInput();
  
  int8_t x = GamePad.getXaxisData();
  int8_t y = GamePad.getYaxisData();
  
  if (x != lastX || y != lastY) {
    sentData.x = x;
    sentData.y = y;
    esp_now_send(peerMac, (uint8_t*)&sentData, sizeof(sentData));
    lastX = x;
    lastY = y;
    
    Serial.print("Sent X:");
    Serial.print(x);
    Serial.print(" Y:");
    Serial.println(y);
  }
  delay(15);
}
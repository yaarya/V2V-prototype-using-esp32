// from A: 14:33:5C:30:88:68 
// sending to B: 14:33:5C:30:85:98
#include <esp_now.h>
#include <WiFi.h>

// Define the data structure
typedef struct struct_BSM {
  char type[4];         // Always "BSM"
  float x_position;
  float y_position;
  float speed;
  float acceleration;
  uint32_t timestamp;   // millis()
} struct_BSM;

uint8_t peerMac[] = {0x14, 0x33, 0x5C, 0x30, 0x85, 0x98};

struct_BSM sentData;
struct_BSM receivedData;

const int sendInterval = 2000;  // Send data every 2 seconds
unsigned long lastSendTime = 0;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Packet Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// Callback when data is received
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
  memcpy(&receivedData, incomingData, sizeof(receivedData));
  Serial.println("Received Data:");
  Serial.print("Type: "); Serial.println(receivedData.type);
  Serial.print("X: "); Serial.println(receivedData.x_position);
  Serial.print("Y: "); Serial.println(receivedData.y_position);
  Serial.print("Speed: "); Serial.println(receivedData.speed);
  Serial.print("Accel: "); Serial.println(receivedData.acceleration);
  Serial.print("Timestamp: "); Serial.println(receivedData.timestamp);
  Serial.println();
}

// Generate random values for BSM struct
void generateRandomBSM(struct_BSM &bsm) {
  strncpy(bsm.type, "BSM", 4);        // Always "BSM"
  bsm.x_position = random(0, 10000) / 100.0;      // 0.00 - 100.00
  bsm.y_position = random(0, 10000) / 100.0;      // 0.00 - 100.00
  bsm.speed = random(0, 10000) / 100.0;           // 0.00 - 100.00
  bsm.acceleration = random(-1000, 1000) / 100.0; // -10.00 - 10.00
  bsm.timestamp = millis();                       // Current timestamp
}

void setup() {
  Serial.begin(115200);
  Serial.println("ESP-NOW Bidirectional Communication");
  
  // Set device as Wi-Fi Station
  WiFi.mode(WIFI_STA);
  
  // Print MAC address
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Init Failed");
    return;
  }

  // Register callbacks
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);
  
  // Register peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, peerMac, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
  Serial.print("Registered Peer: ");
  for (int i = 0; i < 6; i++) {
    Serial.print(peerMac[i], HEX);
    if (i < 5) Serial.print(":");
  }
  Serial.println("\n");
}

void loop() {
  if (millis() - lastSendTime >= sendInterval) {
    generateRandomBSM(sentData);
    
    // Send message via ESP-NOW
    esp_err_t result = esp_now_send(peerMac, (uint8_t *) &sentData, sizeof(sentData));
    
    if (result == ESP_OK) {
      Serial.println("Sent Data:");
      Serial.print("Type: "); Serial.println(sentData.type);
      Serial.print("X: "); Serial.println(sentData.x_position);
      Serial.print("Y: "); Serial.println(sentData.y_position);
      Serial.print("Speed: "); Serial.println(sentData.speed);
      Serial.print("Accel: "); Serial.println(sentData.acceleration);
      Serial.print("Timestamp: "); Serial.println(sentData.timestamp);
      Serial.println();
    } else {
      Serial.println("Error sending data");
    }
    
    lastSendTime = millis();
  }
}
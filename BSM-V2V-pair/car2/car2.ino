#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE
#include <DabbleESP32.h>
#include <esp_now.h>
#include <WiFi.h>

#define TRIG_PIN 12 // GPIO12 (P12)
#define ECHO_PIN 13 // GPIO13 (P13)
#define LED_PIN 2 // Onboard LED (GPIO2)
#define THRESHOLD 30 // Distance threshold in cm

//Right motor
int enableRightMotor = 22; 
int rightMotorPin1 = 16;
int rightMotorPin2 = 17;
//Left motor
int enableLeftMotor = 23;
int leftMotorPin1 = 18;
int leftMotorPin2 = 19;

#define MAX_MOTOR_SPEED 255

const int PWMFreq = 1000; // 1 KHz
const int PWMResolution = 8;
const int rightMotorPWMSpeedChannel = 4;
const int leftMotorPWMSpeedChannel = 5;

// ESP-NOW Configuration
uint8_t peerMac[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF}; // TO BE CHANGED PER BOARD

// BSM struct definition
typedef struct __attribute__((packed)) struct_BSM {
  char type[4]; // always "BSM"
  float distance; // distance from collision
  int speed; // joystick Y-axis (-7 to 7)
  float acceleration; // speed change over time
  uint32_t timestamp; // milliseconds
} struct_BSM;

struct_BSM bsmData;
unsigned long lastSendTime = 0;
const long SEND_INTERVAL = 100; // Send every 100ms

// Variables for acceleration calculation
int previousSpeed = 0;
unsigned long previousTime = 0;

// Reception variables
volatile float currentDistance = 100.0; // Default safe distance
unsigned long previousMillis = 0; // For LED blinking
unsigned long lastDataTime = 0; // Last received message time
const long BLINK_INTERVAL = 250; // Blink interval (ms)
const long DATA_TIMEOUT = 1000; // Timeout (ms)

void rotateMotor(int rightMotorSpeed, int leftMotorSpeed) {
  if (rightMotorSpeed < 0) {
    digitalWrite(rightMotorPin1, LOW);
    digitalWrite(rightMotorPin2, HIGH);    
  } else if (rightMotorSpeed > 0) {
    digitalWrite(rightMotorPin1, HIGH);
    digitalWrite(rightMotorPin2, LOW);      
  } else {
    digitalWrite(rightMotorPin1, LOW);
    digitalWrite(rightMotorPin2, LOW);      
  }
  
  if (leftMotorSpeed < 0) {
    digitalWrite(leftMotorPin1, LOW);
    digitalWrite(leftMotorPin2, HIGH);    
  } else if (leftMotorSpeed > 0) {
    digitalWrite(leftMotorPin1, HIGH);
    digitalWrite(leftMotorPin2, LOW);      
  } else {
    digitalWrite(leftMotorPin1, LOW);
    digitalWrite(leftMotorPin2, LOW);      
  }
  
  ledcWrite(rightMotorPWMSpeedChannel, abs(rightMotorSpeed));
  ledcWrite(leftMotorPWMSpeedChannel, abs(leftMotorSpeed));  
}

void setUpPinModes() {
  pinMode(enableRightMotor, OUTPUT);
  pinMode(rightMotorPin1, OUTPUT);
  pinMode(rightMotorPin2, OUTPUT);
  
  pinMode(enableLeftMotor, OUTPUT);
  pinMode(leftMotorPin1, OUTPUT);
  pinMode(leftMotorPin2, OUTPUT);

  ledcSetup(rightMotorPWMSpeedChannel, PWMFreq, PWMResolution);
  ledcSetup(leftMotorPWMSpeedChannel, PWMFreq, PWMResolution);  
  ledcAttachPin(enableRightMotor, rightMotorPWMSpeedChannel);
  ledcAttachPin(enableLeftMotor, leftMotorPWMSpeedChannel); 

  rotateMotor(0, 0); 
}

float getDistanceCM() {
  static unsigned long lastMeasurementTime = 0;
  static float lastValidDistance = 100.0; // Default to max distance
  const unsigned long MEASURE_INTERVAL = 15; // Minimum time between measurements 

  unsigned long currentTime = millis();
  
  // Only take new measurement if 15ms have passed
  if (currentTime - lastMeasurementTime >= MEASURE_INTERVAL) {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    long duration = pulseIn(ECHO_PIN, HIGH, 8000); // 8ms timeout
    
    if (duration == 0) {
      lastValidDistance = 100.0; // Timeout value
    } else {
      lastValidDistance = duration * 0.0343 / 2.0; // Calculate distance
    }
    lastMeasurementTime = currentTime; // Update measurement timer
  }
  return lastValidDistance; // Return cached value
}

// Map joystick values to motor speeds to prevent low-power beeping
int mapY(int value) {
  int sign = value > 0 ? 1 : -1;
  switch (abs(value)) {
    case 1: return sign * 75;
    case 2: return sign * 100;
    case 3: return sign * 125;
    case 4: return sign * 150;
    case 5: return sign * 175;
    case 6: return sign * 200;
    case 7: return sign * 225;
    default: return 0;
  }
}

int mapX(int value) {
  int sign = value > 0 ? 1 : -1;
  switch (abs(value)) {
    case 3: return sign * 100;
    case 4: return sign * 125;
    case 5: return sign * 150;
    case 6: return sign * 175;
    case 7: return sign * 200;
    default: return 0;
  }
}

// void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
//   if (status != ESP_NOW_SEND_SUCCESS) {
//     Serial.println("ESP-NOW Send Failed");
//   }
// }

void OnDataRecv(const uint8_t *mac, const uint8_t *data, int len) {
  struct_BSM receivedData;
  memcpy(&receivedData, data, sizeof(receivedData));
  lastDataTime = millis();
  currentDistance = receivedData.distance;

  Serial.print("RX - Type: ");
  Serial.print(receivedData.type);
  Serial.print(" | Dist: ");
  Serial.print(receivedData.distance);
  Serial.print("cm | Speed: ");
  Serial.print(receivedData.speed);
  Serial.print(" | Accel: ");
  Serial.print(receivedData.acceleration);
  Serial.print(" | TS: ");
  Serial.println(receivedData.timestamp);
}

void setup() {
  setUpPinModes();
  Dabble.begin("Car2");
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);  
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  strncpy(bsmData.type, "BSM", 4); // Initialize BSM struct

  // Initialize ESP-NOW
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) return;
  
  // Register peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, peerMac, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  esp_now_add_peer(&peerInfo);

  // Optional: Register send callback
  // esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv); // Register receiver callback
}

void loop() {
  Dabble.processInput();
  float distance = getDistanceCM();
  
  int left = 0;
  int right = 0;

  int x = GamePad.getXaxisData();
  int y = GamePad.getYaxisData();

  // Only process if joystick is not centered
  if (x != 0 || y != 0) {
    int mapped_x = mapX(x);
    int mapped_y = mapY(y);

    // Calculate motor speeds
    left = mapped_y + mapped_x;
    right = mapped_y - mapped_x;

    // Constrain to valid motor range
    left = constrain(left, -MAX_MOTOR_SPEED, MAX_MOTOR_SPEED);
    right = constrain(right, -MAX_MOTOR_SPEED, MAX_MOTOR_SPEED);
  }

  // Collision avoidance - stop forward movement if obstacle detected
  if (distance < 30) {
    int avgSpeed = (left + right) / 2;
    if (avgSpeed > 0) { // Only stop if net movement is forward
      left = 0;
      right = 0;
    }
  }
  // Serial.print("Distance: ");
  // Serial.print(distance);
  // Serial.println(" cm");

  // ESP-NOW Transmission (non-blocking) with acceleration calculation
  unsigned long currentTime = millis();
  if (currentTime - lastSendTime >= SEND_INTERVAL) {
    lastSendTime = currentTime;
    
    // Compute acceleration only when sending
    if (previousTime != 0) {
      float deltaT = (currentTime - previousTime) / 1000.0; // Convert to seconds
      bsmData.acceleration = (y - previousSpeed) / deltaT;
    } else {
      bsmData.acceleration = 0.0; // First reading
    }
    
    // Update state for next calculation
    previousSpeed = y;
    previousTime = currentTime;

    // Populate BSM struct
    bsmData.distance = distance;
    bsmData.speed = y;
    bsmData.timestamp = currentTime;

    esp_now_send(peerMac, (uint8_t *)&bsmData, sizeof(bsmData));
  }

  rotateMotor(right, left);

  // LED control logic
  unsigned long currentMillis = millis();

  if (currentMillis - lastDataTime > DATA_TIMEOUT) {
    digitalWrite(LED_PIN, LOW);  // turn LED off if no recent data
  }
  else if (currentDistance < THRESHOLD) {
    // Obstacle detected - blink
    if (currentMillis - previousMillis >= BLINK_INTERVAL) {
      previousMillis = currentMillis;
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    }
  }
  else {
    digitalWrite(LED_PIN, LOW);  // Clear when safe
  }
}
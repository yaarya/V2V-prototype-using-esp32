#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE
#include <DabbleESP32.h>
#include <esp_now.h>
#include <WiFi.h>

#define TRIG_PIN 12 // GPIO12 (P12)
#define ECHO_PIN 13 // GPIO13 (P13)

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
uint8_t peerMac[] = {0x14, 0x33, 0x5C, 0x30, 0x85, 0x98};

typedef struct __attribute__((packed)) {
  float distance;
} DistanceData;

DistanceData distanceData;
unsigned long lastSendTime = 0;
const long SEND_INTERVAL = 100; // Send every 100ms

// void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
//   if (status != ESP_NOW_SEND_SUCCESS) {
//     Serial.println("ESP-NOW Send Failed");
//   }
// }


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

  //Set up PWM for speed
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

void setup() {
  setUpPinModes();
  Dabble.begin("MyBluetoothCar"); 
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);  

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
    if (avgSpeed > 0) {  // Only stop if net movement is forward
      left = 0;
      right = 0;
    }
  }
  // Serial.print("Distance: ");
  // Serial.print(distance);
  // Serial.println(" cm");

  // ESP-NOW Transmission (non-blocking)
  unsigned long currentMillis = millis();
  if (currentMillis - lastSendTime >= SEND_INTERVAL) {
    lastSendTime = currentMillis;
    distanceData.distance = distance;
    esp_now_send(peerMac, (uint8_t *)&distanceData, sizeof(distanceData));
  }

  rotateMotor(right, left);
}

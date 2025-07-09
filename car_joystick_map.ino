#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE
#include <DabbleESP32.h>

#define TRIG_PIN 12  // GPIO12 (P12)
#define ECHO_PIN 13  // GPIO13 (P13)

//Right motor
int enableRightMotor = 22; 
int rightMotorPin1 = 16;
int rightMotorPin2 = 17;
//Left motor
int enableLeftMotor = 23;
int leftMotorPin1 = 18;
int leftMotorPin2 = 19;

#define MAX_MOTOR_SPEED 255

const int PWMFreq = 1000; /* 1 KHz */
const int PWMResolution = 8;
const int rightMotorPWMSpeedChannel = 4;
const int leftMotorPWMSpeedChannel = 5;

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
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  // 10 microsecond pulse output
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // measure time 
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);  // 30ms timeout
  // convert to distance
  float distance;
  if (duration == 0) {
    // No object detected within 100 cm
    distance = 300.0;
  } else {
    // Calculate distance
    distance = duration * 0.0343 / 2.0;
  }
  return distance;
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
  if (distance < 15) {
    int avgSpeed = (left + right) / 2;
    if (avgSpeed > 0) {  // Only stop if net movement is forward
      left = 0;
      right = 0;
    }
  }

  rotateMotor(right, left);
}
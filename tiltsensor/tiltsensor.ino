#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <MPU6050_light.h>

MPU6050 mpu(Wire);
LiquidCrystal_I2C lcd(0x27, 20, 4);

const int buttonPin = 3;
float angleOffset = 0;
bool buttonState = false;
bool lastButtonState = false;
float smoothedAngle = 0;
float alpha = 0.1;
unsigned long lastUpdateTime = 0;
const unsigned long updateInterval = 300; // ms

void showIntroScreen() {
lcd.clear();
// Center "Angle Meter"
const char* line1 = "ANGLE METER";
lcd.setCursor((20 - strlen(line1)) / 2, 0);
lcd.print(line1);

// Center "Made By"
const char* line2 = "MADE BY";
lcd.setCursor((20 - strlen(line2)) / 2, 1);
lcd.print(line2);

// Center "Electro Gadget"
const char* line3 = "MAHI SARWATE";
lcd.setCursor((20 - strlen(line3)) / 2, 2);
lcd.print(line3);

delay(2000);
lcd.clear();
}

void setup() {
pinMode(buttonPin, INPUT_PULLUP);
Wire.begin();
Serial.begin(9600);
lcd.init();
lcd.backlight();

showIntroScreen();

// Center "Initializing..."
const char* initMsg = "Initializing...";
lcd.setCursor((20 - strlen(initMsg)) / 2, 1);
lcd.print(initMsg);

mpu.begin();
delay(1000);
mpu.calcOffsets(true, true);

lcd.clear();

// Center "MPU6050 Ready"
const char* readyMsg = "MPU6050 READY";
lcd.setCursor((20 - strlen(readyMsg)) / 2, 1);
lcd.print(readyMsg);

delay(1000);
lcd.clear();

// Print static UI elements: title on row 0
lcd.setCursor(4, 0);
lcd.print("ANGLE METER");

// Angle and Offset labels shifted to rows 2 and 3 respectively
lcd.setCursor(0, 2);
lcd.print("ANGLE: ");

lcd.setCursor(0, 3);
lcd.print("OFFSET:");

smoothedAngle = 0;
}

void loop() {
mpu.update();
buttonState = (digitalRead(buttonPin) == LOW);
if (buttonState && !lastButtonState) angleOffset = mpu.getAngleX();
lastButtonState = buttonState;

float rawAngle = mpu.getAngleX() - angleOffset;
smoothedAngle = alpha * rawAngle + (1 - alpha) * smoothedAngle;

unsigned long currentTime = millis();
if (currentTime - lastUpdateTime >= updateInterval) {
lastUpdateTime = currentTime;

// Update only the angle value field on row 2
lcd.setCursor(7, 2);
char angleStr[6];
snprintf(angleStr, sizeof(angleStr), "%4d%c ", (int)smoothedAngle, 223);
lcd.print(angleStr);

// Update only the offset field on row 3
lcd.setCursor(8, 3);
char offsetStr[6];
snprintf(offsetStr, sizeof(offsetStr), "%4d%c ", (int)angleOffset, 223);
lcd.print(offsetStr);
}
}

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <LedControl.h>
#include <TM1637Display.h>
#include <DHT.h>
#include <Servo.h>

void controlDoorsWithPotentiometers();
void updateDoorStatusDisplay();
void handleFrontGear();
void handleReverseGear();
void clearLC();
void Welcome();
bool isAnyDoorOpen();
void drawSplitX();
void checkWindowButton();
void updateLightStatusIfChanged();
void checkEnvironmentStatus();
void renderCurrentPage();
void handleMenuButtons();
void updateSeatbeltStatus();
void handleDoorLockButton();
#define CLOSE_DOORS_BUTTON_PIN 26

// Servo Definitions
Servo servoFL;
Servo servoFR;
Servo servoRL;
Servo servoRR;
Servo servoLW;
Servo servoRW;

#define SERVO_FL_PIN 11
#define SERVO_FR_PIN 13
#define SERVO_RL_PIN 10
#define SERVO_RR_PIN 12

// Potentiometer Pins
#define POT_FL_PIN A4 // Front Left
#define POT_FR_PIN A6 // Front Right
#define POT_RL_PIN A5 // Rear Left
#define POT_RR_PIN A7 // Rear Right

// LCD Definitions (Two I2C Displays)
#define I2C_ADDR1 0x27
#define I2C_ADDR2 0x26
LiquidCrystal_I2C lcd1(I2C_ADDR1, 20, 4); // Main Display (address 0x27)
LiquidCrystal_I2C lcd2(I2C_ADDR2, 20, 4); // Secondary Display (address 0x26)

// DHT Sensor
#define DHTPIN 7
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// LED Matrix
LedControl lc = LedControl(51, 52, 53, 2); // DIN, CLK, CS, 2 devices

bool showWarmingUp = false;
int speed = 0;

// System Pins
#define startInput 22
#define startOUT 23
#define buzzerPin 25
int startFLAG = 0;
bool lastButtonState = HIGH;

// Door lock system
bool doorsLocked = false;
bool lastLockButtonState = HIGH;

// Joystick
int VRx = A0;
int VRy = A1;
int SW = 24;

// Ultrasonic Sensor
const int trigPin = A2;
const int echoPin = A3;
long duration;
int distance;

// PIR Sensor
#define PIR_PIN 6
bool motionActive = false;
unsigned long motionStartTime = 0;
bool motionWarningDisplayed = false;

// 7-Segment Display
#define CLK 2
#define DIO 3
TM1637Display display(CLK, DIO);

// Custom LCD Characters
byte xMiddle[] = {B00000, B00000, B00000, B00000, B00000, B11011, B00100, B11011};
byte xBottomLeft[] = {B00011, B00110, B01100, B11000, B10000, B00000, B00000, B00000};
byte xBottomRight[] = {B11000, B01100, B00110, B00011, B00001, B00000, B00000, B00000};

byte botRight[] = {
    B11100,
    B11100,
    B00100,
    B11100,
    B11000,
    B00000,
    B00000,
    B00000};
byte topLeft[] = {
    B00000, B00011, B00111, B00110, B00100, B00111, B00111, B00111};
byte topRight[] = {
    B00000,
    B11000,
    B11100,
    B01100,
    B00100,
    B11100,
    B11100,
    B11100};
byte botLeft[] = {
    B00111,
    B00111,
    B00100,
    B00111,
    B00011,
    B00000,
    B00000,
    B00000};

byte openDoor1[] = {
    B00000,
    B00000,
    B00000,
    B00011,
    B00110,
    B01100,
    B11000,
    B00000};
byte openDoor2[] = {
    B00000,
    B00000,
    B00000,
    B11000,
    B01100,
    B00110,
    B00011,
    B00000};

bool motionPreviouslyDetected = false;
unsigned long lastEnvWarningTime = 0;
const unsigned long envWarningInterval = 5000;
static bool showingWarning = false;

#define POT_PIN A8 // Potentiometer connected to analog pin A8
#define LED_PIN 30 // LEDs controlled from digital pin 30
#define LED_PIN2 29
#define LED_PIN3 31
#define THRESHOLD 50 // Adjust this value as needed

// Mirros
#define WINDOWPIN 34
#define RWLED 42
#define LWLED 40
#define ACCELERATE_BUTTON_PIN 33
bool windowIsClosed = false;
bool lastWindowButtonState = HIGH;
unsigned long lastWindowToggleTime = 0;
const unsigned long debounceDelay = 50; // 50ms debounce
#define SERVO_LW_PIN 8
#define SERVO_RW_PIN 9

// Menu
#define BUTTON_LEFT 35
#define BUTTON_RIGHT 36
int currentPage = 0;
char gearState = 'N';
float currentTemp = NAN;
float currentHum = NAN;
String lightStatus = "OFF";
String warning1 = "";
String warning2 = "";
bool shouldRefreshPage = true;
String lastRenderedLightStatus = "";
float lastRenderedTemp = NAN;
float lastRenderedHum = NAN;
char lastRenderedGear = ' ';
String lastRenderedWarning1 = "";
String lastRenderedWarning2 = "";
int lastRenderedPage = -1;

// Cruise control
#define CRUISE_BUTTON_PIN 28
bool cruiseControlEnabled = false;
bool lastCruiseButtonState = HIGH;
int cruiseSpeed = 0;

static bool speedExceeded120 = false;

// seatbelt
#define SEATBELT_DRIVER_PIN A9
#define SEATBELT_PASSENGER_PIN A10
bool seatbeltDriverFastened = false;
bool seatbeltPassengerFastened = false;

// warning seat belt
bool seatbeltWarningActive = false;
unsigned long seatbeltWarningStartTime = 0;
unsigned long lastBeepTime = 0;
bool hasAcceleratedBefore = false;

bool inReverseGear = false;

// brake
#define BRAKE_BUTTON_PIN 32
bool isBraking = false;

void setup()
{
  Serial.begin(9600);

  // Initialize LCDs with backlight
  lcd1.begin(20, 4);
  lcd2.begin(20, 4);

  // Initialize servos
  servoFL.attach(SERVO_FL_PIN);
  servoFR.attach(SERVO_FR_PIN);
  servoRL.attach(SERVO_RL_PIN);
  servoRR.attach(SERVO_RR_PIN);
  servoLW.attach(SERVO_LW_PIN);
  servoRW.attach(SERVO_RW_PIN);

  // Set initial servo positions (doors open)
  servoFL.write(0);
  servoFR.write(0);
  servoRL.write(0);
  servoRR.write(0);

  // Initialize other components
  pinMode(buzzerPin, OUTPUT);
  pinMode(startInput, INPUT_PULLUP);
  pinMode(startOUT, OUTPUT);
  digitalWrite(startOUT, LOW);
  pinMode(SW, INPUT_PULLUP);
  dht.begin();
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  display.setBrightness(0x0f);
  pinMode(PIR_PIN, INPUT);
  pinMode(CLOSE_DOORS_BUTTON_PIN, INPUT_PULLUP);

  // Create custom characters
  lcd1.createChar(2, xMiddle);
  lcd1.createChar(3, xBottomLeft);
  lcd1.createChar(4, xBottomRight);

  lcd2.createChar(0, topLeft);  // Slot 0
  lcd2.createChar(1, topRight); // Slot 1
  lcd2.createChar(2, botLeft);  // Slot 2
  lcd2.createChar(3, botRight); // Slot 3
  lcd2.createChar(4, openDoor1);
  lcd2.createChar(5, openDoor2);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  pinMode(LED_PIN2, OUTPUT);
  digitalWrite(LED_PIN2, LOW);
  pinMode(LED_PIN3, OUTPUT);
  digitalWrite(LED_PIN3, LOW);

  pinMode(ACCELERATE_BUTTON_PIN, INPUT_PULLUP);

  pinMode(RWLED, OUTPUT);
  pinMode(LWLED, OUTPUT);
  pinMode(WINDOWPIN, INPUT_PULLUP);

  // Initialize LED matrix
  lc.shutdown(0, false); // Wake up displays from power-saving mode
  lc.shutdown(1, false);
  lc.setIntensity(0, 8); // Set brightness level (0 is min, 15 is max)
  lc.setIntensity(1, 8);
  lc.clearDisplay(0);
  lc.clearDisplay(1);

  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);

  pinMode(CRUISE_BUTTON_PIN, INPUT_PULLUP);
  pinMode(BRAKE_BUTTON_PIN, INPUT_PULLUP);

  lcd1.clear();
  lcd2.clear();

  delay(1000);
}

void loop()
{
  bool currentButtonState = digitalRead(startInput);

  // Toggle car ON/OFF
  if (lastButtonState == HIGH && currentButtonState == LOW)
  {
    delay(50); // debounce
    if (digitalRead(startInput) == LOW)
    {
      startFLAG = !startFLAG;
      if (startFLAG)
      {
        lcd1.backlight();
        lcd1.setCursor(5, 1);
        lcd1.print("Warming UP...");
        drawSplitX();
        // for (int i = 3; i >= 0; i--)
        // {
        // display.showNumberDec(i);
        //   delay(1000);
        //  }
        display.clear();
        lcd1.clear();
        // Ensure backlight is on
        lcd2.backlight();
        // Welcome();
      }
      else
      {
        lcd1.clear();
        lcd2.clear();
        lcd1.noBacklight(); // Turn off backlight when car is off
        lcd2.noBacklight();
        
      }
      while (digitalRead(startInput) == LOW)
        delay(10);
    }
  }
  lastButtonState = currentButtonState;

  // Run when car is ON
  if (startFLAG)
  {
    if (startFLAG && currentPage == 0 && shouldRefreshPage && !motionWarningDisplayed)
    {
      renderCurrentPage(); // Home screen refresh
      shouldRefreshPage = false;
    }

    handleMenuButtons();
    checkWindowButton();
    updateSeatbeltStatus();

    updateLightStatusIfChanged();
    if (!showingWarning && !inReverseGear)
    {
      drawSplitX();
    }
    if (currentPage == 0)
    {
      checkEnvironmentStatus(); // only show temp/humidity on home
    }

    handleDoorLockButton();

    // Control doors with potentiometers and update LCD2
    controlDoorsWithPotentiometers();
    updateDoorStatusDisplay();

    // Read joystick values
    int x = analogRead(VRx);
    int y = analogRead(VRy);

    char newGear = 'N';
    if (x > 450 && x < 550 && y > 900)
      newGear = 'D';
    else if (x > 450 && x < 550 && y < 100)
      newGear = 'R';

    // Handle gear transitions BEFORE calling any gear-related functions
    if (gearState == 'D' && newGear == 'N')
    {
      speed = 0;
      display.clear();
      cruiseControlEnabled = false;
      cruiseSpeed = 0;
      shouldRefreshPage = true;
    }

    if (newGear != gearState)
    {
      gearState = newGear;
      shouldRefreshPage = true;
      inReverseGear = (gearState == 'R');
    }

    // Now call the appropriate gear function AFTER updating gearState
    if (gearState == 'D')
    {
      handleFrontGear();
    }
    else if (gearState == 'R')
    {
      handleReverseGear();
    }
    // Update temperature/humidity
    float temp = dht.readTemperature();
    float hum = dht.readHumidity();
    if (!isnan(temp))
      currentTemp = temp;
    if (!isnan(hum))
      currentHum = hum;

    // Update light status
    if (analogRead(POT_PIN) > 512)
      lightStatus = "ON";
    else if (analogRead(POT_PIN) > THRESHOLD)
      lightStatus = "FOG";
    else
      lightStatus = "OFF";

    int xl = analogRead(VRx);

    static unsigned long lastBlinkTimeRW = 0;
    static bool ledStateRW = false;
    static unsigned long lastBlinkTimeLW = 0;
    static bool ledStateLW = false;
    unsigned long now = millis();

    // Joystick to the right → blink RWLED
    if (xl > 800)
    {
      if (now - lastBlinkTimeRW >= 300)
      {
        ledStateRW = !ledStateRW;
        digitalWrite(RWLED, ledStateRW);
        lastBlinkTimeRW = now;
      }
    }
    else
    {
      digitalWrite(RWLED, LOW); // Turn off if not right
    }

    // Joystick to the left → blink LWLED
    if (xl < 200)
    {
      if (now - lastBlinkTimeLW >= 300)
      {
        ledStateLW = !ledStateLW;
        digitalWrite(LWLED, ledStateLW);
        lastBlinkTimeLW = now;
      }
    }
    else
    {
      digitalWrite(LWLED, LOW); // Turn off if not left
    }

    if (digitalRead(SW) == LOW)
    {
      tone(buzzerPin, 1000); // Play horn while pressed
    }
    else
    {
      noTone(buzzerPin); // Stop horn when released
    }

    if (startFLAG && shouldRefreshPage)
    {
      renderCurrentPage();
      shouldRefreshPage = false;
    }
  }
}
void drawSplitX()
{
  lc.clearDisplay(0);
  lc.clearDisplay(1);
  lc.setLed(0, 0, 4, true);
  lc.setLed(0, 7, 4, true);
  lc.setLed(0, 1, 5, true);
  lc.setLed(0, 6, 5, true);
  lc.setLed(0, 2, 6, true);
  lc.setLed(0, 5, 6, true);
  lc.setLed(0, 3, 7, true);
  lc.setLed(0, 4, 7, true);
  lc.setLed(1, 3, 0, true);
  lc.setLed(1, 4, 0, true);
  lc.setLed(1, 2, 1, true);
  lc.setLed(1, 1, 2, true);
  lc.setLed(1, 0, 3, true);
  lc.setLed(1, 5, 1, true);
  lc.setLed(1, 6, 2, true);
  lc.setLed(1, 7, 3, true);
}

void handleDoorLockButton()
{
  bool currentLockButtonState = digitalRead(CLOSE_DOORS_BUTTON_PIN);

  if (lastLockButtonState == HIGH && currentLockButtonState == LOW)
  {
    delay(50); // debounce

    if (isAnyDoorOpen() && !doorsLocked)
    {
      lcd2.clear();
      lcd2.setCursor(0, 1);
      lcd2.print("Close all doors plz");
      tone(buzzerPin, 1000, 500);
      delay(3000);
      lcd2.clear();
      updateDoorStatusDisplay();
    }
    else
    {
      doorsLocked = !doorsLocked;

      if (doorsLocked)
      {
        // Lock → close all doors
        servoFL.write(0);
        servoFR.write(0);
        servoRL.write(0);
        servoRR.write(0);
        tone(buzzerPin, 1500, 200);
      }
      else
      {
        tone(buzzerPin, 1000, 200);
      }

      updateDoorStatusDisplay();
      shouldRefreshPage = true;
    }

    delay(300); // final debounce
  }

  lastLockButtonState = currentLockButtonState;
}

void controlDoorsWithPotentiometers()
{
  if (doorsLocked)
  {

    return;
  }

  int angleFL = map(analogRead(POT_FL_PIN), 0, 1023, 0, 90);
  int angleFR = map(analogRead(POT_FR_PIN), 0, 1023, 0, 90);
  int angleRL = map(analogRead(POT_RL_PIN), 0, 1023, 0, 90);
  int angleRR = map(analogRead(POT_RR_PIN), 0, 1023, 0, 90);

  servoFL.write(angleFL);
  servoFR.write(angleFR);
  servoRL.write(angleRL);
  servoRR.write(angleRR);

  delay(20); // Prevent servo jitter
}

void updateDoorStatusDisplay()
{
  lcd2.setCursor(7, 0);
  lcd2.print(doorsLocked ? "LOCKED" : "UNLOCK");

  // Front Left Door
  lcd2.setCursor(0, 1);
  lcd2.print("FL:");
  bool flOpen = !doorsLocked && analogRead(POT_FL_PIN) > 10;
  lcd2.print(flOpen ? "OP" : "CL");

  // Front Right Door
  lcd2.setCursor(14, 1);
  lcd2.print("FR:");
  bool frOpen = !doorsLocked && analogRead(POT_FR_PIN) > 10;
  lcd2.print(frOpen ? "OP" : "CL");

  // Rear Left Door
  lcd2.setCursor(0, 2);
  lcd2.print("RL:");
  bool rlOpen = !doorsLocked && analogRead(POT_RL_PIN) > 10;
  lcd2.print(rlOpen ? "OP" : "CL");

  // Rear Right Door
  lcd2.setCursor(14, 2);
  lcd2.print("RR:");
  bool rrOpen = !doorsLocked && analogRead(POT_RR_PIN) > 10;
  lcd2.print(rrOpen ? "OP" : "CL");

  // Draw car (always center on bottom two rows)
  lcd2.setCursor(9, 2);
  lcd2.write(0); // Top of car
  lcd2.setCursor(10, 2);
  lcd2.write(1); // Bottom of car
  lcd2.setCursor(9, 3);
  lcd2.write(2); // Top of car
  lcd2.setCursor(10, 3);
  lcd2.write(3); // Bottom of car

  // --- Clear door icon positions ---
  lcd2.setCursor(8, 2);
  lcd2.print(" "); // rear left
  lcd2.setCursor(11, 2);
  lcd2.print(" "); // rear right
  lcd2.setCursor(8, 3);
  lcd2.print(" "); // front left
  lcd2.setCursor(11, 3);
  lcd2.print(" "); // front right

  // --- Draw door icons if open ---
  if (flOpen)
  {
    lcd2.setCursor(8, 2);
    lcd2.write(4); // front left
  }
  if (frOpen)
  {
    lcd2.setCursor(11, 2);
    lcd2.write(5); // front right
  }
  if (rlOpen)
  {
    lcd2.setCursor(8, 3);
    lcd2.write(4); // rear left
  }
  if (rrOpen)
  {
    lcd2.setCursor(11, 3);
    lcd2.write(5); // rear right
  }
}

bool isAnyDoorOpen()
{
  return (analogRead(POT_FL_PIN) > 10 ||
          analogRead(POT_FR_PIN) > 10 ||
          analogRead(POT_RL_PIN) > 10 ||
          analogRead(POT_RR_PIN) > 10);
}

void handleFrontGear()
{

  static unsigned long lastSpeedUpdate = 0;
  int motion = digitalRead(PIR_PIN);
  unsigned long now = millis();

  if (motion == HIGH && !motionActive)
  {
    motionActive = true;
    motionStartTime = now;
    motionWarningDisplayed = true; // <- NEW

    // Show warning
    lcd1.clear();
    lcd1.setCursor(7, 1);
    lcd1.print("WARNING");
    lcd1.setCursor(2, 2);
    lcd1.print("Object detected");

    // LED matrix warning
    clearLC();
    lc.setLed(0, 0, 7, true);
    lc.setLed(0, 1, 7, true);
    lc.setLed(0, 2, 7, true);
    lc.setLed(0, 3, 7, true);
    lc.setLed(0, 4, 7, true);
    lc.setLed(0, 6, 7, true);

    tone(buzzerPin, 1000, 100);
    delay(260);
    tone(buzzerPin, 1000, 100);
  }

  // Motion ends: reset everything
  if (motion == LOW && motionActive && (now - motionStartTime > 3000))
  {
    lcd1.clear();
    clearLC();
    motionActive = false;
    motionWarningDisplayed = false; // <- NEW
    shouldRefreshPage = true;
    lastRenderedPage = -1;
    renderCurrentPage();
  }

  bool currentCruiseButtonState = digitalRead(CRUISE_BUTTON_PIN);

  if (lastCruiseButtonState == HIGH && currentCruiseButtonState == LOW)
  {
    cruiseControlEnabled = !cruiseControlEnabled;

    if (cruiseControlEnabled)
      cruiseSpeed = speed; // lock the current speed

    tone(buzzerPin, cruiseControlEnabled ? 1500 : 1000, 100);
    shouldRefreshPage = true;
  }

  lastCruiseButtonState = currentCruiseButtonState;
  // Handle speed simulation
  now = millis();
  bool isAccelerating = digitalRead(ACCELERATE_BUTTON_PIN) == LOW;
  bool brakePressed = digitalRead(BRAKE_BUTTON_PIN) == LOW;
  isBraking = brakePressed;
  if (cruiseControlEnabled)
  {
    if (brakePressed)
    {
      // Brake lowers speed quickly, cruise resumes after release
      if (now - lastSpeedUpdate >= 50)
      {
        int brakePower = cruiseControlEnabled ? 15 : 10;
        speed -= brakePower;
        if (speed < 0)
          speed = 0;
        
        lastSpeedUpdate = now;
      }
    }
    else if (isAccelerating)
    {
      if (now - lastSpeedUpdate >= 200)
      {
        speed = min(speed + 5, 240);
        lastSpeedUpdate = now;
      }
    }
    else if (speed < cruiseSpeed && now - lastSpeedUpdate >= 200)
    {
      speed = min(speed + 2, cruiseSpeed);
      lastSpeedUpdate = now;
    }
    else if (speed > cruiseSpeed && now - lastSpeedUpdate >= 400)
    {
      speed = max(speed - 2, cruiseSpeed);
      lastSpeedUpdate = now;
    }
  }
  else // cruise not enabled
  {
    if (brakePressed)
    {
      if (now - lastSpeedUpdate >= 100)
      {
        speed = max(speed - 5, 0);
        lastSpeedUpdate = now;
      }
    }
    else if (isAccelerating)
    {
      if (now - lastSpeedUpdate >= 200)
      {
        speed = min(speed + 5, 240);
        lastSpeedUpdate = now;
      }
    }
    else if (now - lastSpeedUpdate >= 400)
    {
      speed = max(speed - 2, 0);
      lastSpeedUpdate = now;
    }
  }

  if (speed > 120 && !speedExceeded120)
  {
    // Play two short tones
    tone(buzzerPin, 1000, 100);
    delay(150);
    tone(buzzerPin, 1000, 100);
    delay(150);
    noTone(buzzerPin);
    speedExceeded120 = true; // Set flag so it won't repeat
  }

  if (speed <= 120)
  {
    speedExceeded120 = false; // Reset the flag if speed drops back
  }
  isAccelerating = digitalRead(ACCELERATE_BUTTON_PIN) == LOW;

  if (isAccelerating && !hasAcceleratedBefore)
  {
    hasAcceleratedBefore = true;

    if (!seatbeltDriverFastened)
    {
      seatbeltWarningActive = true;
      seatbeltWarningStartTime = millis();
      lastBeepTime = 0; // allow immediate beep
    }
  }
  if (seatbeltWarningActive)
  {
    // Cancel the warning immediately if driver fastens seatbelt
    if (seatbeltDriverFastened)
    {
      seatbeltWarningActive = false;
      noTone(buzzerPin);
    }
    else
    {
      unsigned long now = millis();
      if (now - seatbeltWarningStartTime < 30000)
      { // 30 seconds duration
        if (now - lastBeepTime >= 5000)
        { // every 5 seconds
          tone(buzzerPin, 1000, 100);
          delay(200);
          tone(buzzerPin, 1000, 100);
          lastBeepTime = now;
        }
      }
      else
      {
        seatbeltWarningActive = false;
        noTone(buzzerPin);
      }
    }
  }

  // Show speed on 7-segment display
  display.showNumberDec(speed);

  delay(50);
}

void handleReverseGear()
{
  lc.shutdown(0, false);
  lc.shutdown(1, false);
  lc.setIntensity(0, 8);
  lc.setIntensity(1, 8);

  // Draw static reverse arrows (once)
  lc.clearDisplay(0);
  lc.clearDisplay(1);

  // Left arrow
  lc.setLed(0, 7, 3, true);
  lc.setLed(0, 7, 1, true);
  lc.setLed(0, 7, 2, true);
  lc.setLed(0, 6, 1, true);
  lc.setLed(0, 6, 2, true);
  lc.setLed(0, 6, 3, true);
  lc.setLed(0, 5, 1, true);
  lc.setLed(0, 5, 2, true);
  lc.setLed(0, 4, 1, true);
  lc.setLed(0, 4, 2, true);
  lc.setLed(0, 3, 1, true);
  lc.setLed(0, 2, 1, true);

  // Right arrow
  lc.setLed(1, 7, 4, true);
  lc.setLed(1, 7, 5, true);
  lc.setLed(1, 7, 6, true);
  lc.setLed(1, 6, 4, true);
  lc.setLed(1, 6, 5, true);
  lc.setLed(1, 6, 6, true);
  lc.setLed(1, 5, 5, true);
  lc.setLed(1, 5, 6, true);
  lc.setLed(1, 4, 5, true);
  lc.setLed(1, 4, 6, true);
  lc.setLed(1, 3, 6, true);
  lc.setLed(1, 2, 6, true);

  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  if (!isnan(temp))
    currentTemp = temp;
  if (!isnan(hum))
    currentHum = hum;

  while (true)
  {
    controlDoorsWithPotentiometers();
    updateDoorStatusDisplay();
    // checkEnvironmentStatus();

    if (analogRead(POT_PIN) > 512)
      lightStatus = "ON";
    else if (analogRead(POT_PIN) > THRESHOLD)
      lightStatus = "FOG";
    else
      lightStatus = "OFF";

    updateSeatbeltStatus();
    checkWindowButton();
    handleDoorLockButton();
    updateLightStatusIfChanged();
    handleMenuButtons();
    renderCurrentPage();
    int x = analogRead(VRx);
    int y = analogRead(VRy);

    if (!(x > 450 && x < 550 && y < 100))
      break;

    // Ultrasonic distance measurement
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH, 30000);
    distance = duration ? duration * 0.034 / 2 : 999;

    display.clear();
    if (distance < 100)
      display.showNumberDec((int)distance);

    // Clear only previous beeping zone
    lc.setLed(0, 1, 7, false);
    lc.setLed(0, 2, 7, false);
    lc.setLed(0, 3, 7, false);
    lc.setLed(0, 5, 7, false);
    lc.setLed(0, 6, 7, false);
    lc.setLed(0, 7, 7, false);
    lc.setLed(1, 1, 0, false);
    lc.setLed(1, 2, 0, false);
    lc.setLed(1, 3, 0, false);
    lc.setLed(1, 5, 0, false);
    lc.setLed(1, 6, 0, false);
    lc.setLed(1, 7, 0, false);

    // Distance warning with beeps and minimal dots
    if (distance > 125)
    {
      noTone(buzzerPin);
    }
    else if (distance > 100)
    {
      lc.setLed(0, 1, 7, true);
      lc.setLed(1, 1, 0, true);
      noTone(buzzerPin);
    }
    else if (distance > 75)
    {
      lc.setLed(0, 2, 7, true);
      lc.setLed(1, 2, 0, true);
      tone(buzzerPin, 1000);
      delay(500);
      noTone(buzzerPin);
      delay(500);
    }
    else if (distance > 50)
    {
      lc.setLed(0, 3, 7, true);
      lc.setLed(1, 3, 0, true);
      tone(buzzerPin, 1000);
      delay(500);
      noTone(buzzerPin);
      delay(500);
    }
    else if (distance > 25)
    {
      lc.setLed(0, 5, 7, true);
      lc.setLed(1, 5, 0, true);
      tone(buzzerPin, 1000);
      delay(250);
      noTone(buzzerPin);
      delay(250);
    }
    else if (distance > 10)
    {
      lc.setLed(0, 6, 7, true);
      lc.setLed(1, 6, 0, true);
      tone(buzzerPin, 1000);
      delay(100);
      noTone(buzzerPin);
      delay(100);
    }
    else
    {
      lc.setLed(0, 7, 7, true);
      lc.setLed(1, 7, 0, true);
      tone(buzzerPin, 1000);
    }

    delay(50);
  }

  lcd1.clear();
  display.clear();
  lc.clearDisplay(0);
  lc.clearDisplay(1);
  noTone(buzzerPin);
}

void checkEnvironmentStatus()
{
  static float lastTemp = NAN;
  static float lastHum = NAN;

  static unsigned long lastSensorCheck = 0;
  static unsigned long lastWarningCheck = 0;
  static unsigned long warningStartTime = 0;

  const unsigned long sensorInterval = 1000;       // Refresh temp/hum every 1s if changed
  const unsigned long warningCheckInterval = 5000; // Check for new warnings every 5s
  const unsigned long warningDuration = 3000;      // Show warning for 4s

  unsigned long now = millis();
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  // --- Update temperature/humidity only if changed significantly ---
  if (now - lastSensorCheck >= sensorInterval)
  {
    if (!isnan(temp) && (isnan(lastTemp) || abs(temp - lastTemp) > 0.5))
    {
      lastTemp = temp;
      currentTemp = temp;
      shouldRefreshPage = true; // Trigger screen refresh
    }

    if (!isnan(hum) && (isnan(lastHum) || abs(hum - lastHum) > 1.0))
    {
      lastHum = hum;
      currentHum = hum;
      shouldRefreshPage = true;
    }

    lastSensorCheck = now;
  }

  // --- Check for new warnings ---
  if (!showingWarning && (now - lastWarningCheck >= warningCheckInterval))
  {
    warning1 = "";
    warning2 = "";

    if (!isnan(temp))
    {
      if (temp < 5)
        warning1 = "Temp too LOW!";
      else if (temp > 35)
        warning1 = "Temp too HIGH!";
    }

    if (!isnan(hum))
    {
      if (hum < 20)
        warning2 = "Humidity LOW!";
      else if (hum > 80)
        warning2 = "Humidity HIGH!";
    }

    if ((warning1 != "" || warning2 != "") && (!inReverseGear))
    {
      // Display warnings
      lcd1.setCursor(0, 2); // Always show warning1 on line 2
      lcd1.print("                    ");
      lcd1.setCursor(0, 2);
      lcd1.print(warning1);

      lcd1.setCursor(0, 3); // warning2 (humidity) goes below it
      lcd1.print("                    ");
      lcd1.setCursor(0, 3);
      lcd1.print(warning2);

      clearLC();
      lc.setLed(0, 0, 7, true);
      lc.setLed(0, 1, 7, true);
      lc.setLed(0, 2, 7, true);
      lc.setLed(0, 3, 7, true);
      lc.setLed(0, 4, 7, true);
      lc.setLed(0, 6, 7, true);

      showingWarning = true;
      warningStartTime = now;
    }

    lastWarningCheck = now;
  }

  // --- Clear warning after timeout ---
  if (showingWarning && (now - warningStartTime >= warningDuration))
  {
    lcd1.setCursor(0, 3);
    lcd1.print("                    ");
    lcd1.setCursor(0, 2);
    lcd1.print("                    ");
    clearLC();
    showingWarning = false;
    warning1 = "";
    warning2 = "";
  }
}

void updateLightStatusIfChanged()
{
  static int lastState = -1; // 0 = OFF, 1 = FOG, 2 = ON

  int potValue = analogRead(POT_PIN);
  int currentState = 0;

  if (potValue > 512)
    currentState = 2;
  else if (potValue > THRESHOLD)
    currentState = 1;
  else
    currentState = 0;

  if (currentState == lastState)
    return; // no change → skip everything
  lastState = currentState;

  switch (currentState)
  {
  case 2: // ON
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(LED_PIN2, HIGH);
    digitalWrite(LED_PIN3, HIGH);
    lcd1.setCursor(0, 1);
    lcd1.print("             ");
    lcd1.setCursor(0, 1);
    lcd1.print("Light: ON");
    break;
  case 1: // FOG
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(LED_PIN2, LOW);
    digitalWrite(LED_PIN3, HIGH);
    lcd1.setCursor(0, 1);
    lcd1.print("           ");
    lcd1.setCursor(0, 1);
    lcd1.print("Light: FOG");
    break;
  case 0: // OFF
    digitalWrite(LED_PIN, LOW);
    digitalWrite(LED_PIN2, LOW);
    digitalWrite(LED_PIN3, LOW);
    lcd1.setCursor(0, 1);
    lcd1.print("           ");
    lcd1.setCursor(0, 1);
    lcd1.print("Light: OFF");
    break;
  }
}

void checkWindowButton()
{
  static bool pressRegistered = false;
  bool currentState = digitalRead(WINDOWPIN);

  if (currentState == LOW && !pressRegistered)
  {
    pressRegistered = true;
    windowIsClosed = !windowIsClosed;

    if (windowIsClosed)
    {
      servoRW.write(0);
      servoLW.write(0);
    }
    else
    {
      servoRW.write(90);
      servoLW.write(90);
    }

    shouldRefreshPage = true; // <- Add this line to trigger immediate update
  }
  else if (currentState == HIGH)
  {
    pressRegistered = false;
  }
}

void handleMenuButtons()
{
  static bool lastLeft = HIGH;
  static bool lastRight = HIGH;

  bool currentLeft = digitalRead(BUTTON_LEFT);
  bool currentRight = digitalRead(BUTTON_RIGHT);

  if (lastRight == HIGH && currentRight == LOW)
  {
    currentPage = min(currentPage + 1, 1);
    renderCurrentPage();
  }

  if (lastLeft == HIGH && currentLeft == LOW)
  {
    currentPage = 0;
    renderCurrentPage();
  }

  lastLeft = currentLeft;
  lastRight = currentRight;
}

void renderHomePage(char gear, float temperature, float humidity, const String &lightStatus, const String &warningMsg1, const String &warningMsg2)
{
  lcd1.clear();

  // Line 0: Gear and Humidity
  lcd1.setCursor(0, 0);
  lcd1.print("Gear:");
  lcd1.print(gear);
  lcd1.print("      "); // clear leftover characters
  lcd1.setCursor(13, 0);
  lcd1.print("H:");
  lcd1.print(humidity, 1);
  lcd1.print(" "); // in case value shrinks

  // Line 1: Light and Temperature
  lcd1.setCursor(0, 1);
  lcd1.print("Light: ");
  lcd1.print(lightStatus);
  lcd1.print("      ");
  lcd1.setCursor(13, 1);
  lcd1.print("       "); // clear old T value

  lcd1.setCursor(13, 1);
  lcd1.print("T:");
  lcd1.print(temperature, 1);
  lcd1.print(" ");

  // Line 2: Warnings or clear space
  lcd1.setCursor(0, 2);
  if (warningMsg1 != "")
    lcd1.print(warningMsg1);
  else
    lcd1.print("                    "); // clear line

  // Line 3: Humidity warning (if any)
  lcd1.setCursor(0, 3);
  if (warningMsg2 != "")
    lcd1.print(warningMsg2);
  else
    lcd1.print("                    "); // clear line
}

void renderCurrentPage()
{
  if (currentPage != lastRenderedPage)
  {
    lcd1.clear(); // Only clear on actual page switch
  }

  if (currentPage == 0)
  {
    bool needsUpdate = false;

    // Don't compare with lastRenderedPage here!
    if (gearState != lastRenderedGear)
      needsUpdate = true;
    if (currentTemp != lastRenderedTemp)
      needsUpdate = true;
    if (currentHum != lastRenderedHum)
      needsUpdate = true;
    if (lightStatus != lastRenderedLightStatus)
      needsUpdate = true;
    if (warning1 != lastRenderedWarning1 || warning2 != lastRenderedWarning2)
      needsUpdate = true;

    // Force render if page changed
    if (currentPage != lastRenderedPage)
      needsUpdate = true;

    if (needsUpdate)
    {
      renderHomePage(gearState, currentTemp, currentHum, lightStatus, warning1, warning2);
      lastRenderedGear = gearState;
      lastRenderedTemp = currentTemp;
      lastRenderedHum = currentHum;
      lastRenderedLightStatus = lightStatus;
      lastRenderedWarning1 = warning1;
      lastRenderedWarning2 = warning2;
      lastRenderedPage = currentPage;
    }
  }
  else if (currentPage == 1)
  {
    if (lastRenderedPage != 1)
      lcd1.clear();

    lcd1.setCursor(0, 0);
    lcd1.print("Mirrors: ");
    lcd1.print(windowIsClosed ? "Closed " : "Open   ");

    lcd1.setCursor(0, 1);
    if (cruiseControlEnabled)
    {
      lcd1.print("Cruise: ON (");
      lcd1.print(cruiseSpeed);
      lcd1.print(")   ");
    }
    else
    {
      lcd1.print("Cruise: OFF       "); // pad with spaces to overwrite leftovers
    }
    lcd1.setCursor(0, 2);
    lcd1.print("Driver Belt: ");
    lcd1.print(seatbeltDriverFastened ? "Yes" : "No ");

    lcd1.setCursor(0, 3);
    lcd1.print("Passgr Belt: ");
    lcd1.print(seatbeltPassengerFastened ? "Yes" : "No ");

    lastRenderedPage = currentPage;
  }
}

void updateSeatbeltStatus()
{
  static bool lastDriverStatus = false;
  static bool lastPassengerStatus = false;

  int driverValue = analogRead(SEATBELT_DRIVER_PIN);
  int passengerValue = analogRead(SEATBELT_PASSENGER_PIN);

  bool driverFastened = driverValue > 900;
  bool passengerFastened = passengerValue > 900;

  if (driverFastened != lastDriverStatus || passengerFastened != lastPassengerStatus)
  {
    seatbeltDriverFastened = driverFastened;
    seatbeltPassengerFastened = passengerFastened;
    lastDriverStatus = driverFastened;
    lastPassengerStatus = passengerFastened;
    shouldRefreshPage = true;
  }
}

void Welcome()
{
  lcd1.clear();

  // X logo on main display
  lcd1.setCursor(9, 1);
  lcd1.write(2);
  lcd1.setCursor(8, 2);
  lcd1.write(3);
  lcd1.setCursor(8, 1);
  lcd1.write(4);
  lcd1.setCursor(10, 2);
  lcd1.write(4);
  lcd1.setCursor(10, 1);
  lcd1.write(3);

  String line1 = "Welcome Driver";
  String line2 = "Drive safe with X";

  int melody[] = {440, 440, 622, 440, 440, 659, 587, 523, 493, 440};
  int noteDur[] = {300, 300, 300, 300, 300, 300, 300, 300, 300, 600};

  int maxSteps = max(max(line1.length(), line2.length()), 10);

  for (int step = 0; step < maxSteps; step++)
  {
    if (step < line1.length())
    {
      lcd1.setCursor(3 + step, 0);
      lcd1.print(line1[step]);
    }
    if (step < line2.length())
    {
      lcd1.setCursor(1 + step, 3);
      lcd1.print(line2[step]);
    }
    if (step < 10)
      tone(buzzerPin, melody[step], noteDur[step]);
    delay(225);
  }

  noTone(buzzerPin);
  delay(1500);
  lcd1.clear();
}

void clearLC()
{
  for (int j = 0; j < 8; j++)
  {
    for (int i = 0; i < 8; i++)
    {
      lc.setLed(0, j, i, false);
      lc.setLed(1, j, i, false);
    }
  }
}
// === NetrOn Full Arduino Code with Serial Plotter Dashboard and Bluetooth App Output ===
// Sensors: LM35, ADXL345, HC-05 Bluetooth
// Outputs: Red & Green LED status 
// Inputs: Voltage Divider (A1)

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <SoftwareSerial.h>

// === Sensor & Communication Setup ===
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(123);
SoftwareSerial BT(11, 10);  // RX, TX (connect HC-05 TX to pin 11)

// === Pin Definitions ===
const int lm35Pin = A0;     // LM35 temp sensor
const int voltagePin = A1;  // Voltage divider (battery monitor)
const int greenLED = 6;
const int redLED = 7;

void setup() {
  Serial.begin(9600);    // For Serial Plotter
  BT.begin(9600);        // For HC-05 Bluetooth

  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);

  // === ADXL345 Setup ===
  if (!accel.begin()) {
    Serial.println("ADXL345 not detected");
    BT.println("ADXL345 not detected");
    while (1);
  }
  accel.setRange(ADXL345_RANGE_16_G);

  Serial.println("System Initialized");
  BT.println("System Initialized");
}

void loop() {
  // === 1. Read LM35 Temperature ===
  int tempReading = analogRead(lm35Pin);
  float tempVoltage = tempReading * 5.0 / 1023.0;
  float temperatureC = tempVoltage * 100;  // 10mV/°C

  // === 2. Read Battery Voltage via Divider ===
  int rawVolt = analogRead(voltagePin);
  float batteryVoltage = rawVolt * 5.0 / 1023.0 * 2;  // Assumes divider halves voltage

  // === 3. Read Accelerometer ===
  sensors_event_t event;
  accel.getEvent(&event);
  float x = event.acceleration.x;
  float y = event.acceleration.y;
  float z = event.acceleration.z;
  float vibration = abs(x) + abs(y) + abs(z);

  // === 4. Safety Check ===
  bool isSafe = (temperatureC < 60) && (batteryVoltage > 6.5) && (vibration < 25);

  // === 5. LED Indication ===
  if (isSafe) {
    digitalWrite(greenLED, HIGH);
    digitalWrite(redLED, LOW);
  } else {
    digitalWrite(greenLED, LOW);
    digitalWrite(redLED, HIGH);
  }

  // === 6. Send to Bluetooth ===
  BT.print("Temp: "); BT.print(temperatureC); BT.print(" °C, ");
  BT.print("Battery: "); BT.print(batteryVoltage); BT.print(" V, ");
  BT.print("Vibration: "); BT.println(vibration);

  // === 7. Serial Plotter Output (with labels for readability) ===
  Serial.print("Temperature: "); Serial.print(temperatureC); Serial.print(", ");
  Serial.print("Battery: "); Serial.print(batteryVoltage); Serial.print(", ");
  Serial.print("Vibration: "); Serial.println(vibration);

  delay(1000);
}


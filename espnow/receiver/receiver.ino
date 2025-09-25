#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// BLE UUIDs
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLECharacteristic *pCharacteristic;

// Mock joystick values (replace with actual analog reads)
int joystickX = 0;
int joystickY = 0;

void setup() {
  Serial.begin(115200);

  // Initialize BLE
  BLEDevice::init("JoystickSender");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_NOTIFY
                    );

  pCharacteristic->addDescriptor(new BLE2902());
  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->start();

  Serial.println("BLE joystick sender started...");
}

void loop() {
  // Read your joystick values (replace analog pins with your hardware)
  joystickX = analogRead(34);  // Example: X axis
  joystickY = analogRead(35);  // Example: Y axis

  // Convert values to a string or a byte array
  String valueStr = String(joystickX) + "," + String(joystickY);
  pCharacteristic->setValue(valueStr.c_str());
  pCharacteristic->notify(); // Send to connected device

  delay(100); // Adjust rate
}

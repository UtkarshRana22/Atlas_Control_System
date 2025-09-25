#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

const int VRx = 32;
const int VRy = 33;
const int SW  = 25;

// BLE service and characteristic UUIDs
#define SERVICE_UUID        "12345678-1234-1234-1234-1234567890ab"
#define CHARACTERISTIC_UUID "abcd1234-5678-1234-5678-abcdef123456"

BLECharacteristic *pCharacteristic;
unsigned long previousMillis = 0;
const long interval = 30; // send every 30ms

void setup() {
  Serial.begin(115200);
  pinMode(SW, INPUT_PULLUP);

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
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->start();

  Serial.println("BLE Joystick Sender Ready...");
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    int xValue = analogRead(VRx);
    int yValue = analogRead(VRy);
    int buttonState = digitalRead(SW);

    int xMapped = map(xValue, 0, 4095, -100, 100);
    int yMapped = map(yValue, 0, 4095, -100, 100);

    int data[3] = {xMapped, yMapped, buttonState};
    pCharacteristic->setValue((uint8_t*)&data, sizeof(data));
    pCharacteristic->notify(); // send to central

    Serial.print("Sent -> X: "); Serial.print(xMapped);
    Serial.print(" | Y: "); Serial.print(yMapped);
    Serial.print(" | Button: "); Serial.println(buttonState);
  }
}

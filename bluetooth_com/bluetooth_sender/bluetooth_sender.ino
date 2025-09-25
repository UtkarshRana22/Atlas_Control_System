#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEClient.h>
#include <BLEAddress.h>

const int VRx = 32;
const int VRy = 33;
const int SW  = 25;   // Joystick button pin
const int LED = 2;    // Status LED pin

// Server info
BLEAddress serverAddress("44:1D:64:BE:24:E2");  // Target server MAC
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLEClient*  pClient;
BLERemoteCharacteristic* pRemoteCharacteristic;
bool connected = false;

// Client callbacks for connect/disconnect events
class MyClientCallbacks : public BLEClientCallbacks {
  void onConnect(BLEClient* pClient) {
    Serial.println("✅ Connected to server");
    connected = true;
    digitalWrite(LED, HIGH);
  }

  void onDisconnect(BLEClient* pClient) {
    Serial.println("❌ Disconnected from server");
    connected = false;
    digitalWrite(LED, LOW);
  }
};

void setup() {
  Serial.begin(115200);

  // Initialize joystick pins
  pinMode(VRx, INPUT);
  pinMode(VRy, INPUT);
  pinMode(SW, INPUT);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  BLEDevice::init("ESP32_Client");

  pClient = BLEDevice::createClient();
  pClient->setClientCallbacks(new MyClientCallbacks());
  Serial.println("BLE Client created");

  if (pClient->connect(serverAddress)) {
    Serial.println("Connected to server");

    BLERemoteService* pRemoteService = pClient->getService(SERVICE_UUID);
    if (!pRemoteService) {
      Serial.println("Failed to find service.");
      pClient->disconnect();
      return;
    }

    pRemoteCharacteristic = pRemoteService->getCharacteristic(CHARACTERISTIC_UUID);
    if (!pRemoteCharacteristic) {
      Serial.println("Failed to find characteristic.");
      pClient->disconnect();
      return;
    }

    Serial.println("Ready to send joystick data!");
  } else {
    Serial.println("Failed to connect to server.");
  }
}

void loop() {
  if (connected && pRemoteCharacteristic && pRemoteCharacteristic->canWrite()) {
    int xValue = analogRead(VRx);
    int yValue = analogRead(VRy);
    int buttonState = digitalRead(SW);

    int xMapped = map(xValue, 0, 4095, -2047, 2047);
    int yMapped = map(yValue, 0, 4095, -2047, 2047);

    // Format data as "x,y,z"
    String data = String(xMapped) + "," + String(yMapped) + "," + String(buttonState);
    pRemoteCharacteristic->writeValue(data.c_str());

    Serial.println("Sent joystick data: " + data);
  }

  delay(100); // Send every 100ms
}

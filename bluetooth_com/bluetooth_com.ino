#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <stdlib.h> 

// UUIDs for first service
#define SERVICE_UUID1        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID1 "beb5483e-36e1-4688-b7f5-ea07361b26a8"

// UUIDs for second service
#define SERVICE_UUID2        "6d68efe5-04b6-4a85-abc3-2d95a8a2d01d"
#define CHARACTERISTIC_UUID2 "f3641400-00b0-4240-ba50-05ca45bf8abc"

// Callback for first service
class Service1Callbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) override {
      String value = pCharacteristic->getValue();
      if (value.length() > 0) {
        Serial.print("[Service1] Received Value: ");
        Serial.println(value.c_str());
 const char* numbers = value.c_str(); // C-style string pointer
    int arr[3];                   // Array to store parsed integers
    int i = 0;

    const char* ptr = value.c_str();    // Pointer to traverse the string

    while (*ptr) {                // Loop until null terminator
        arr[i++] = strtol(ptr, (char**)&ptr, 10); // Convert to int
        if (*ptr == ',') ptr++;                    // Skip comma
    }

    // Print the array
    for(int j = 0; j < i; j++) {
        Serial.println(arr[j]);
    }
 
  
      }
    }

    void onRead(BLECharacteristic *pCharacteristic) override {
      String response = "Service1 says hello!";
      pCharacteristic->setValue(response.c_str());
      Serial.println("[Service1] Phone requested data");
    }
};

// Callback for second service
class Service2Callbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) override {
      String value = pCharacteristic->getValue();
      if (value.length() > 0) {
        Serial.print("[Service2] Received Value: ");
        Serial.println(value.c_str());
      }
    }

    void onRead(BLECharacteristic *pCharacteristic) override {
      String response = "Service2 responding!";
      pCharacteristic->setValue(response.c_str());
      Serial.println("[Service2] Phone requested data");
    }
};

// Server callbacks
class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) override {
    Serial.println("Device connected");
    digitalWrite(2,HIGH);
  }

  void onDisconnect(BLEServer* pServer) override {
    Serial.println("Device disconnected, restarting advertising...");
    pServer->startAdvertising();
    digitalWrite(2,LOW);
  }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");
  pinMode(2,OUTPUT);

  BLEDevice::init("MyESP32");
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // -------- Service 1 --------
  BLEService *pService1 = pServer->createService(SERVICE_UUID1);
  BLECharacteristic *pCharacteristic1 = pService1->createCharacteristic(
                                          CHARACTERISTIC_UUID1,
                                          BLECharacteristic::PROPERTY_READ |
                                          BLECharacteristic::PROPERTY_WRITE
                                        );
  pCharacteristic1->setValue("Hello World!ESP32");
  pCharacteristic1->setCallbacks(new Service1Callbacks());
  pService1->start();

  // -------- Service 2 --------
  BLEService *pService2 = pServer->createService(SERVICE_UUID2);
  BLECharacteristic *pCharacteristic2 = pService2->createCharacteristic(
                                          CHARACTERISTIC_UUID2,
                                          BLECharacteristic::PROPERTY_READ |
                                          BLECharacteristic::PROPERTY_WRITE
                                        );
  pCharacteristic2->setValue("Second service ready");
  pCharacteristic2->setCallbacks(new Service2Callbacks());
  pService2->start();

  // -------- Advertising --------
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID1);
  pAdvertising->addServiceUUID(SERVICE_UUID2);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();

  Serial.println("BLE server is ready! Connect from your phone.");
}

void loop() {
  delay(2000);
}

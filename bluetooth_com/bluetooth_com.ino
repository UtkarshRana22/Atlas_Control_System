#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <stdlib.h> 
#include <math.h> 

// UUIDs for first service
#define SERVICE_UUID1        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID1 "beb5483e-36e1-4688-b7f5-ea07361b26a8"

// UUIDs for second service
#define SERVICE_UUID2        "6d68efe5-04b6-4a85-abc3-2d95a8a2d01d"
#define CHARACTERISTIC_UUID2 "f3641400-00b0-4240-ba50-05ca45bf8abc"



// ----- Pins
const int dir1Pin = 12;  // Motor 1 direction(w2) left wheel back
const int pwm1Pin = 13;  // Motor 1 speed (PWM)
const int dir2Pin = 27;  // Motor 2 direction(w3) right wheel back
const int pwm2Pin = 14;  // Motor 2 speed (PWM)

const int dir3Pin=25;
const int pwm3Pin=26;


const int DEADZONE_MIN = 100;  // adjusted center - 15
const int DEADZONE_MAX = 145;  // adjusted center + 15

const float r = 20.3;   
const float L = 28.637;   
float omega = 0.0;   
int tempx=0;
int tempy=0;
int tempz=0;


void stop_(){
analogWrite(pwm1Pin,0);
analogWrite(pwm2Pin,0);
analogWrite(pwm3Pin,0);



}

void motor_direction(float w1,float w2,float w3,int pwm1,int pwm2,int pwm3){
  if(w1!=0){
if(w1>0){
 digitalWrite(dir3Pin,HIGH);
 
}else{
 digitalWrite(dir3Pin,LOW);
}
analogWrite(pwm3Pin,pwm1);

  }
  if(w2!=0){
if(w2>0){
  digitalWrite(dir1Pin,HIGH);

 

}else{
digitalWrite(dir1Pin,LOW);

}
analogWrite(pwm1Pin,pwm2);
}


if(w3!=0){
if(w3>0){
digitalWrite(dir2Pin,HIGH);

}else{
digitalWrite(dir2Pin,LOW);

}
analogWrite(pwm2Pin,pwm3);

}
}


// Apply deadzone to joystick input
int applyDeadzone(int value) {
    if (value >= DEADZONE_MIN && value <= DEADZONE_MAX) {
        return 0; // within deadzone
    } 
    return value; // outside deadzone, return as-is
}

// Compute wheel speeds for 3-wheel omni bot
void computeWheelSpeeds(int X, int Y,int Z,float &w1, float &w2, float &w3,int potVal) {
    omega=map(Z,2047,-2047,-0.689,0.689);
    w1 = (1.0 * Y + L * omega) / r;
    w2 = (-0.866 * X - 0.5 * Y + L * omega) / r;
    w3 = (0.866 * X - 0.5 * Y + L * omega) / r;
    
    float maxW = fmax(fabs(w1), fmax(fabs(w2), fabs(w3)));

   float r1 = w1 / maxW;
   float r2 = w2 / maxW;
   float r3 = w3 / maxW;

  int pwm1 = int(fabs(r1) * potVal);
  int pwm2 = int(fabs(r2) * potVal);
  int pwm3 = int(fabs(r3) * potVal);

   


   motor_direction(w1,w2,w3,pwm1,pwm2,pwm3);
}

// Determine approximate movement direction
String getDirection(float w1, float w2, float w3) {
    // If all wheels are zero, robot is stopped
    Serial.println(w1);
    Serial.println(w2);
    Serial.println(w3);
    if (w1 == 0 && w2 == 0 && w3 == 0) return "Stopped";

    // Forward/backward check (based on majority wheels)
    if (w1==0 && w2 > 0 && w3 < 0){
       return "Forward";
    };
    if (w1==0 && w2 < 0 && w3 >0) {
      
      return "Backward";
      }
    
    ;

    // Sideways check
    if (w1 > 0 && w2>0) return "Right";
    if (w1 < 0 && w2< 0) return "Left";
    

    return "Diagonal/Complex"; // Any other combination
}



// Callback for first service
class Service1Callbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) override {
      String value = pCharacteristic->getValue();
      if (value.length() > 0) {
        Serial.print("[Service1] Received Value: ");
        Serial.println(value.c_str());
 const char* numbers = value.c_str(); // C-style string pointer
    int arr[6];                   // Array to store parsed integers
    int i = 0;

    const char* ptr = value.c_str();    // Pointer to traverse the string
    Serial.println(value.c_str());
    while (*ptr) {                // Loop until null terminator
        arr[i++] = strtol(ptr, (char**)&ptr, 10); // Convert to int
        if (*ptr == ',') ptr++;                    // Skip comma
    }


  
  int X = applyDeadzone(arr[0]);
  int Y = applyDeadzone(arr[1]);
  int Z=applyDeadzone(arr[3]);
  int potval=arr[4];

  if(arr[5]==0){
    stop_();
  }

 if (X == 0 && Y == 0 && Z == 0) {
            stop_();
        } else if (X != tempx || Y != tempy || Z != tempz) {
            // Compute new wheel speeds only if any axis changed
            float w1, w2, w3;
            computeWheelSpeeds(X, Y, Z, w1, w2, w3, potval);  
            Serial.print(" | Direction: "); Serial.println(getDirection(w1, w2, w3));
        }

        // Update previous values
        tempx = X;
        tempy = Y;
        tempz = Z;
  
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





    // Direction pins
  pinMode(dir1Pin, OUTPUT);
  pinMode(dir2Pin, OUTPUT);
  pinMode(dir3Pin, OUTPUT);
  pinMode(pwm1Pin, OUTPUT);
  pinMode(pwm2Pin, OUTPUT);
  pinMode(pwm3Pin, OUTPUT);

  // Setup PWM channels

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

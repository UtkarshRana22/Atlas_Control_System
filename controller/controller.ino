#include <WiFi.h>
#include <ESPAsyncWebServer.h>

const char* ssid = "ESP32_Joystick";
const char* password = "12345678";

AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);

  // Start Wi-Fi in AP mode
  WiFi.softAP(ssid, password);
  Serial.println("AP Started");
  Serial.println(WiFi.softAPIP());
  pinMode(2,OUTPUT);
  digitalWrite(2,HIGH);

  // Endpoint to receive joystick data
  server.on("/joystick", HTTP_GET, [](AsyncWebServerRequest *request){
    if(request->hasParam("x") && request->hasParam("y")){
      String x = request->getParam("x")->value();
      String y = request->getParam("y")->value();
      Serial.printf("Joystick X: %s Y: %s\n", x.c_str(), y.c_str());
      request->send(200, "text/plain", "OK");
    } else {
      request->send(400, "text/plain", "Missing parameters");
    }
  });

  server.begin();
}

void loop() {
  // Nothing here yet
}

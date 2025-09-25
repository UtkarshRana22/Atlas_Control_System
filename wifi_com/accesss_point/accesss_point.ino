#include <WiFi.h>
#include <WebServer.h>

// --- Access Point credentials ---
const char* ap_ssid = "ESP32-AP";
const char* ap_pass = "12345678";

// --- Web server ---
WebServer server(80);

// --- LED pin ---
#define LED_PIN 2

// === Web Handlers ===
void handleGetData() {
  String response = "{\"status\":\"ok\",\"msg\":\"Hello from ESP32 AP\"}";
  server.send(200, "application/json", response);
}

void handlePostData() {
  if (!server.hasArg("plain")) {
    server.send(400, "text/plain", "No body received");
    return;
  }
  String body = server.arg("plain");
  if(body=="blink"){
    digitalWrite(LED_PIN,LOW);
    delay(5000);
    digitalWrite(LED_PIN,HIGH);
    Serial.println("Received POST: " + body);
    server.send(200, "text/plain", "Data received: " + body);
  
  }else if(body=="off"){
    digitalWrite(LED_PIN,LOW);

  }else if(body=="on"){
    digitalWrite(LED_PIN,HIGH);
  }
  
}

// === Setup ===
void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  // Start Access Point
  delay(5000);
  digitalWrite(LED_PIN, HIGH);
  WiFi.softAP(ap_ssid, ap_pass);

  Serial.println("AP started! IP: ");
  Serial.println(WiFi.softAPIP()); // usually 192.168.4.1
  digitalWrite(LED_PIN, HIGH); // AP mode LED on
  Serial.print("IP:");
  Serial.println(WiFi.softAPIP());
  // Setup server routes
  server.on("/getData", HTTP_GET, handleGetData);
  server.on("/postData", HTTP_POST, handlePostData);

  server.begin();
  Serial.println("🌐 Web server started in AP mode");
}

// === Loop ===
void loop() {
  server.handleClient();
  //Serial.println("active");
}  
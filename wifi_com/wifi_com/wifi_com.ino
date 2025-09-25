#include <WiFi.h>
#include <WebServer.h>

// Wi-Fi credentials
const char* targetSSID = "RNServe";
const char* password   = "rana@1234";

// --- Static IP config ---
IPAddress local_IP(192, 168, 1, 200);
// IPAddress gateway(192, 168, 1, 1);
// IPAddress subnet(255, 255, 255, 0);
// IPAddress primaryDNS(8, 8, 8, 8);
// IPAddress secondaryDNS(8, 8, 4, 4);

// Web server
WebServer server(80);

// LED pin
#define LED_PIN 2

// Storage for target BSSID
uint8_t target_bssid[6] = {0,0,0,0,0,0};

// === Web Handlers ===
void handleGetData() {
  String response = "{\"status\":\"ok\",\"msg\":\"Hello from ESP32\"}";
  server.send(200, "application/json", response);
}

void handlePostData() {
  if (!server.hasArg("plain")) {
    server.send(400, "text/plain", "No body received");
    return;
  }
  String body = server.arg("plain");
  Serial.println("Received POST: " + body);
  server.send(200, "text/plain", "Data received");
}

// === Wi-Fi connect function ===
bool connectToWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  Serial.println("Scanning Wi-Fi networks...");
  int n = WiFi.scanNetworks();
  bool found = false;

  for (int i = 0; i < n; i++) {
    String ssid = WiFi.SSID(i);
    String bssidStr = WiFi.BSSIDstr(i);

    Serial.print(i + 1);
    Serial.print(": ");
    Serial.print(ssid);
    Serial.print(" | RSSI: ");
    Serial.print(WiFi.RSSI(i));
    Serial.print(" dBm | BSSID: ");
    Serial.println(bssidStr);

    if (ssid == targetSSID) {
      memcpy(target_bssid, WiFi.BSSID(i), 6);  // store BSSID
      found = true;
      Serial.print("✅ Found target SSID! BSSID = ");
      Serial.println(bssidStr);
      break;
    }
  }

  if (!found) {
    Serial.println("⚠️ Target SSID not found.");
    return false;
  }

  // Apply static IP
  if (!WiFi.config(local_IP)) {
    Serial.println("⚠️ Failed to configure static IP!");
  }

  // Connect to the target AP using BSSID lock
  Serial.print("Connecting to ");
  Serial.println(targetSSID);
  WiFi.begin(targetSSID, password, 0, target_bssid, true);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 40) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ Connected!");
    Serial.print("ESP32 IP: ");
    Serial.println(WiFi.localIP());
    digitalWrite(LED_PIN, HIGH);
    return true;
  } else {
    Serial.println("\n❌ Failed to connect.");
    digitalWrite(LED_PIN, LOW);
    return false;
  }
}

// === Setup ===
void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  if (connectToWiFi()) {
    server.on("/getData", HTTP_GET, handleGetData);
    server.on("/postData", HTTP_POST, handlePostData);

    server.begin();
    Serial.println("🌐 Web server started");
  }
}

// === Loop ===
void loop() {
  server.handleClient();

  // Auto-reconnect if disconnected
  if (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_PIN, LOW);
    if (connectToWiFi()) {
      server.begin();  // restart server after reconnect
    }
  }
}

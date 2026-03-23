#include <WiFi.h>
#include <WebSocketsClient.h>

const char* ssid     = "YOUR_WIFI";
const char* password = "YOUR_PASSWORD";
const char* serverIP = "192.168.X.X";
const int   serverPort = 3000;
const int   PLAYER_ID  = 1; // Change per device

const int BUTTON_PIN = 4;
const int LED_PIN    = 12;

bool lastButtonState = LOW; // INPUT_PULLUP idles HIGH
bool locked          = false; // locked out after someone buzzes

WebSocketsClient webSocket;

void webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
  if (type == WStype_TEXT) {
    String msg = String((char*)payload);
    Serial.println("WS: " + msg);

    if (msg.indexOf("\"type\":\"reset\"") >= 0) {
      locked = false;
      digitalWrite(LED_PIN, HIGH); // LED off (active LOW)
      Serial.println("Reset — ready to buzz");
    }

    if (msg.indexOf("\"type\":\"buzz\"") >= 0) {
      // Lock out ALL players once anyone buzzes
      locked = true;
      // Only light LED if it's this player
      if (msg.indexOf("\"player\":" + String(PLAYER_ID)) >= 0) {
        digitalWrite(LED_PIN, LOW); // LED on
      }
    }
  }

  if (type == WStype_CONNECTED) {
    Serial.println("WebSocket connected");
  }

  if (type == WStype_DISCONNECTED) {
    Serial.println("WebSocket disconnected — retrying...");
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH); // Start with LED off

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected: " + WiFi.localIP().toString());

  webSocket.begin(serverIP, serverPort, "/");
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);
}

void loop() {
  webSocket.loop();

  bool buttonState = digitalRead(BUTTON_PIN);

  // Falling edge = button pressed; only buzz if not locked out
  if (lastButtonState == LOW && buttonState == LOW && !locked) {
    locked = true;
    String msg = "{\"type\":\"buzz\",\"player\":" + String(PLAYER_ID) + "}";
    webSocket.sendTXT(msg);
    Serial.println("Buzzed! Sent: " + msg);
  }

  lastButtonState = buttonState;
  delay(20); // debounce
}

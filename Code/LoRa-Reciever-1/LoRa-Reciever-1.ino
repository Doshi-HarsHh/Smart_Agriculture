#include <WiFi.h>
#include <HTTPClient.h>
#include <SPI.h>
#include <LoRa.h>

const int LORA_SCK = 14;   // Define your custom SPI pins
const int LORA_MISO = 12; // Adjust these based on your wiring
const int LORA_MOSI = 13;
const int LORA_SS = 15;
const int LORA_RST = 34;
const int LORA_DIO1 = 2;

#define FIREBASE_API_KEY "AIzaSyCWQRC_EzlR8QdqplL6HZwEfQQKQLG-vqA"
#define FIREBASE_DATABASE_URL "https://smart-a-416618-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define WIFI_SSID "Ben Dover"
#define WIFI_PASSWORD "11111111"
#define FIREBASE_PROJECT_ID "smart-a-416618"
#define FIREBASE_COLLECTION "new"

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("LoRa Receiver");

  // Initialize the SPI bus with custom pins
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);

  // Define custom SPI pins for the LoRa module
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO1);  // ss, reset, dio
  
  // Initialize the LoRa module with custom pins
  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  Serial.println("LoRa Initializing...");

  // Wait for LoRa module to initialize
  delay(1000);

  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println("\nConnected to Wi-Fi");
}

void loop() {
  // try to parse packet
  int packetSize = LoRa.parsePacket();

  if (packetSize) {
    Serial.print("Received packet: ");

    // read packet
    int temp = (int)LoRa.read();
    Serial.print(temp);
    Serial.print(" ");
    int humi = (int)LoRa.read();
    Serial.print(humi);
    Serial.println();

    // Construct JSON payload
    String jsonPayload = "{\"fields\": {\"temperature\": {\"doubleValue\": " + String(temp) + "}, \"humidity\": {\"doubleValue\": " + String(humi) + "}}}";

    Serial.print("Sending JSON payload: ");
    Serial.println(jsonPayload);

    // Send HTTP POST request to Firestore REST API
    HTTPClient http;
    http.begin("https://firestore.googleapis.com/v1/projects/" + String(FIREBASE_PROJECT_ID) + "/databases/(default)/documents/" + String(FIREBASE_COLLECTION) + "?key=" + String(FIREBASE_API_KEY));
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(jsonPayload);
    
    if (httpResponseCode > 0) {
      Serial.print("HTTP POST request sent successfully. Response code: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("Error sending HTTP POST request. Response code: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  }
}

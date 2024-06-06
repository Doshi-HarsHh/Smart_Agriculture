#include <WiFi.h>
#include <HTTPClient.h> 
#include <ArduinoJson.h>

// Firebase configuration
#define FIREBASE_API_KEY "AIzaSyCWQRC_EzlR8QdqplL6HZwEfQQKQLG-vqA"
#define FIREBASE_DATABASE_URL "https://smart-a-416618-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define WIFI_SSID "Ben Dover"
#define WIFI_PASSWORD "11111111"
#define FIREBASE_PROJECT_ID "smart-a-416618"
#define FIREBASE_COLLECTION "sensor-data"

// Define LED pin
#define LED_PIN 2 // Change to your LED pin

// Threshold temperature for turning on the LED
#define TEMPERATURE_THRESHOLD 30.0

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Connected to Wi-Fi");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    // Send HTTP GET request to Firestore REST API
    HTTPClient http;
    String url = "https://firestore.googleapis.com/v1/projects/" + String(FIREBASE_PROJECT_ID) + "/databases/(default)/documents/" + String(FIREBASE_COLLECTION) + "?key=" + String(FIREBASE_API_KEY);
    http.begin(url);
    int httpResponseCode = http.GET();

if (httpResponseCode > 0) {
  String payload = http.getString();
  // Serial.println("Raw JSON response:");
  // Serial.println(payload);

  // Parse JSON response
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, payload);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  // Check if the document is an array
  if (!doc.containsKey("documents")) {
    Serial.println("JSON document does not contain 'documents' key");
    return;
  }

  // Get the array of documents
  JsonArray documents = doc["documents"].as<JsonArray>();

  // Loop through each object in the array
  for (JsonObject obj : documents) {
    // Extract temperature value
    JsonObject fields = obj["fields"];
    double temperature = fields["temperature"]["doubleValue"];

    // Print temperature value
    Serial.print("Temperature: ");
    Serial.println(temperature);

    // Check if temperature exceeds threshold
    if (temperature > TEMPERATURE_THRESHOLD) {
      digitalWrite(LED_PIN, HIGH); // Turn on LED
      Serial.println("LED ON");
    } else {
      digitalWrite(LED_PIN, LOW); // Turn off LED
      Serial.println("LED OFF");
    }
  }
} else {
  Serial.print("Error fetching data. HTTP response code: ");
  Serial.println(httpResponseCode);
}

delay(5000);
}
}
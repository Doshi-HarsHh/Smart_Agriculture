#include "DHT.h"
#define DHT11PIN 23
#define AOUT_PIN 22

DHT dht(DHT11PIN, DHT11);
void setup()
{
  
  Serial.begin(115200);
/* Start the DHT11 Sensor */
  dht.begin();
}

void loop()
{
  //DHT
  float humi = dht.readHumidity();
  float temp = dht.readTemperature();
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.print("ºC ");
  Serial.print("Humidity: ");
  Serial.println(humi);
  delay(1000);

//Moisture Sensor
  int value = analogRead(AOUT_PIN); 
  Serial.print("Moisture value: ");
  Serial.println(value);
  delay(500);

}
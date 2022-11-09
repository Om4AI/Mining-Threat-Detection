#include "DHT.h"
#include <ESP8266WiFi.h>
#include <ThingSpeak.h>

#define DHTTYPE DHT11   // DHT 11
#define dht_dpin 5      //D1 pin
int LED = 0;
DHT dht(dht_dpin, DHTTYPE);

const char writeAPIKey[] = "FWW25O8VKLUTERU6";     //  Enter your Write API key from ThingSpeak
long myChannelNumber = 1896394;
WiFiClient client;

#define LED D0   
int buzzer = D2;
int smokeA0 = A0;

// Your threshold value. You might need to change it.
int sensorThres = 500;

void setup() {
   Serial.println("in");
   dht.begin();
   
   pinMode(buzzer, OUTPUT);
   pinMode(smokeA0, INPUT);
   pinMode(BUILTIN_LED, OUTPUT);
   pinMode(LED, OUTPUT);
   Serial.begin(9600);
   WiFi.mode(WIFI_STA);
   WiFi.begin("Lele Bhikari", "Hello@world"); // Enter your WIFI SSID and Password here
   while (WiFi.status() != WL_CONNECTED) 
   {
          delay(500);
          Serial.print(".");
   }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println(WiFi.localIP());
    ThingSpeak.begin(client);
}

void loop() {
   
   delay(1000);
   
   //Reading gas sensor
   int analogSensor = analogRead(smokeA0);
   Serial.print("Gas sensor value: ");
   Serial.println(analogSensor);
   delay(100);
   
   float h = dht.readHumidity();
    float t = dht.readTemperature();      
    float f = dht.readTemperature(true);   
    if (isnan(h) || isnan(t) || isnan(f)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }

     float hif = dht.computeHeatIndex(f, h);

     float hic = dht.computeHeatIndex(t, h, false);
    Serial.print("Current humidity = ");
    Serial.print(h);
    Serial.print("%  ");
    Serial.print("temperature = ");
    Serial.print(t); 
    Serial.print("C  ");
    Serial.print("Heat index: ");
    Serial.print(hic);
    Serial.print("C ");
    Serial.print(hif);
    Serial.println("F");
    
    // Checks if it has reached the threshold value
   if (analogSensor > sensorThres || t > 45 || h > 90)
   {
     tone(buzzer, 6000, 2000);
     digitalWrite(BUILTIN_LED, LOW);
     digitalWrite(LED, HIGH); // turn the LED on
     delay(50000);
   }
   else
   {
     noTone(buzzer);
     digitalWrite(BUILTIN_LED, HIGH); 
     digitalWrite(LED, LOW); // turn the LED off
     delay(1000);
   }
//Uploading Data to thingspeak cloud
    Serial.println("%. Send to Thingspeak.");
    ThingSpeak.writeField(myChannelNumber, 5, analogSensor, writeAPIKey);
    ThingSpeak.writeField(myChannelNumber, 1, t, writeAPIKey);
    ThingSpeak.writeField(myChannelNumber, 2, h, writeAPIKey);
    ThingSpeak.writeField(myChannelNumber, 3, hic, writeAPIKey);
    ThingSpeak.writeField(myChannelNumber, 4, hif, writeAPIKey);
    delay(5000);
}

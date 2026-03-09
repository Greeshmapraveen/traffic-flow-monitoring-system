#include <WiFi.h>

// -------- PIN DEFINITIONS --------
#define IR_SENSOR 34
#define LED 2
#define TRIG 5
#define ECHO 18

// -------- WIFI DETAILS --------
const char* ssid = "greeshma Agepati";
const char* password = "GreeshmaPraveen";

// -------- THINGSPEAK DETAILS --------
String writeAPIKey = "L7UISY092CEKYNPI";

const char* server = "api.thingspeak.com";

WiFiClient client;

int vehicleCount = 0;
bool lastState = HIGH;

void connectWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected!");
}

void setup() {
  Serial.begin(9600);

  pinMode(IR_SENSOR, INPUT);
  pinMode(LED, OUTPUT);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  connectWiFi();
}

void loop() {

  // ---------- IR VEHICLE COUNT ----------
  int currentState = digitalRead(IR_SENSOR);

  if (currentState == LOW && lastState == HIGH) {
    vehicleCount++;
    digitalWrite(LED, HIGH);
    Serial.print("Vehicle Count: ");
    Serial.println(vehicleCount);
  } else {
    digitalWrite(LED, LOW);
  }

  lastState = currentState;

  // ---------- ULTRASONIC DISTANCE ----------
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duration = pulseIn(ECHO, HIGH, 30000);
  float distance = duration * 0.034 / 2;

  Serial.print("Distance: ");
  Serial.println(distance);

  // ---------- SEND DATA TO THINGSPEAK ----------
  if (client.connect(server, 80)) {

    String url = "/update?api_key=" + writeAPIKey +
                 "&field1=" + String(vehicleCount) +
                 "&field2=" + String(distance);

    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: api.thingspeak.com\r\n" +
                 "Connection: close\r\n\r\n");

    Serial.println("Data sent to ThingSpeak");
  }

  client.stop();

  Serial.println("-------------------------");

  delay(15000);   // ThingSpeak minimum 15 seconds
}
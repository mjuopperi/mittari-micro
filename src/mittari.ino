#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include <config.h>

#define debug

OneWire oneWire(2);
DallasTemperature sensors(&oneWire);


void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println("Starting setup...");

  connectToWifi();
  sensors.begin();
  #ifdef debug
  Serial.println("Connecting to server");
  #endif
  while (!sayHelloToServer()) {
    #ifdef debug
    Serial.print(".");
    #endif
    delay(200);
  }
}

void connectToWifi() {
  WiFi.begin(WIFI_SSID, WIFI_PW);
  #ifdef debug
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);
  Serial.println(" ...");
  #endif

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    #ifdef debug
    Serial.print(++i);
    Serial.print(' ');
    #endif
  }

  #ifdef debug
  Serial.println('\n');
  Serial.println("Connection established!");
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());
  #endif
}

bool sayHelloToServer() {
  WiFiClient client;
  HTTPClient http;
  http.begin(client, SERVER_ADDRESS "/hello");

  int httpResponseCode = http.GET();

  if (httpResponseCode == 200) {
    Serial.print("Connected to server! Server says: ");
    String payload = http.getString();
    Serial.print(payload);
    Serial.println();
    http.end();
    return true;
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
    http.end();
    return false;
  }
}

void httpPut(const char* url, const char* data) {
  WiFiClient client;
  HTTPClient http;

  http.begin(client, url);
  http.addHeader("Content-Type", "application/json");
  http.PUT(data);

  http.end();
}

void measureTemperature() {
  sensors.requestTemperatures();
  float temperature = sensors.getTempCByIndex(0);
  char data[26];
  sprintf(data, "{\"temperature\": %.2f}", temperature);
  #ifdef debug
  Serial.println(data);
  #endif
  httpPut(SERVER_ADDRESS "/temperature", data);
}

void loop() {
  delay(500);
  measureTemperature();
}

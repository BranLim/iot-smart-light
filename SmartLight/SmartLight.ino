
#include <Adafruit_NeoPixel.h>
#include <WiFiNINA.h>
#include <ArduinoJson.h>

#define LED_PIN 2
#define LED_COUNT 8


Adafruit_NeoPixel stick(LED_COUNT, LED_PIN, NEO_RGB);
bool toggled = false;

uint8_t LEDColor[3] = {0, 0, 0};

const char *server = "*";
const char *ssid_name = "*";
const char *ssid_pass = "*";

const String api = "/smartlights/api/v1/index";
const String lightConfigApi = "/smartlights/api/v1/lights/status";

WiFiClient client;
int wifiStatus = WL_IDLE_STATUS;

#define Debug 0

void setup() {

#ifdef Debug
  Serial.begin(9600);

  while (!Serial);
  Serial.print("Initialising...");
  Serial.println();
#endif

  stick.begin();

  while (wifiStatus != WL_CONNECTED) {

#ifdef Debug
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid_name);
#endif
    wifiStatus = WiFi.begin(ssid_name, ssid_pass);
    delay(10000);
  }

#ifdef Debug
  Serial.println("WiFi connected");
  Serial.print("Connecting to: ");
  Serial.println( server);
#endif

  if (!client.connect(server, 3000)) {
#ifndef Debug
    Serial.println("Fail to connect to server!");
#endif
    return;

  }

#ifdef Debug
  Serial.println("Checking server is up!");
#endif

  pingServer();

  stick.clear();
  stick.show();

}

void loop() {

  fetchLightConfiguration();
  if (!client.connected()) {
#ifdef Debug
    Serial.println("Client disconnected");
#endif
    client.stop();
  }
  delay(5000);
}

void pingServer() {

#ifdef Debug
  Serial.println("Connecting to server at " + api);
#endif

  client.println("GET " + api + " HTTP/1.1");
  client.println("Connection: close");
  client.println();

#ifdef Debug
  Serial.println("API called.");
  Serial.println(client);
#endif

  delay(10);

  while (client.connected()) {
    if (client.available()) {
      String c = client.readStringUntil('\0');
#ifndef Debug
      Serial.print(c);
      Serial.println();
#endif
      break;
    }
  }

}

void fetchLightConfiguration() {

  if (client.connect(server, 3000)) {
#ifdef Debug
    Serial.println("Connected to server at " + lightConfigApi);
#endif

    client.println("GET " + lightConfigApi + " HTTP/1.1" );
    client.println("Connection: close");
    client.println();

#ifdef Debug
    Serial.println("Called " + lightConfigApi);
#endif
    delay(10);

    while (client.connected()) {
      if (client.available()) {

        String c = client.readStringUntil('\0');
        //Get rid of HTTP Response header
        c.remove(0, c.indexOf("\r\n\r\n") + 1);

        processJsonResponse(c);

        break;
      }
    }
  }
}

void processJsonResponse(String response) {

#ifdef Debug
  Serial.println("Processing server response: ");
  Serial.println(response);
#endif

  DynamicJsonDocument doc(2048);

  DeserializationError error = deserializeJson(doc, response);
  if (error) {
    Serial.println("Deserialisation failed.");
    Serial.println(error.f_str());
    return;
  }

#ifdef Debug
  Serial.println("Deserialise success...");
#endif

  JsonObject content = doc.as<JsonObject>();

  int brightness = content["brightness"].as<int>();
  int state = content["state"].as<int>();
  if (state == 0) {
    stick.clear();
    stick.show();
    return;
  }

  int index = 0;
  JsonArray pixels = content["pixels"].as<JsonArray>();
  for (JsonVariant pixel : pixels) {
    JsonObject led = pixel.as<JsonObject>();
    int red = led["red"].as<int>();
    int green = led["green"].as<int>();
    int blue = led["blue"].as<int>();

    updateLight(index, red, green, blue);
    index++;
  }
  stick.setBrightness(brightness);
  stick.show();
}

void updateLight(int led, int red, int green, int blue) {
  stick.setPixelColor(led, green, red , blue );
}


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

const String api = "/api/v1/index";
const String lightConfigApi = "/api/v1/lights/config";

WiFiClient client;
int wifiStatus = WL_IDLE_STATUS;

void setup() {

  stick.begin();
  stick.show();

  Serial.begin(9600);

  //while (!Serial);
  Serial.print("Initialising...");
  Serial.println();

  while (wifiStatus != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid_name);
    wifiStatus = WiFi.begin(ssid_name, ssid_pass);
    delay(10000);
  }
  Serial.println("WiFi connected");
  Serial.print("Connecting to: ");
  Serial.println( server);


  if (!client.connect(server, 3000)) {
    Serial.println("Fail to connect to server!");
    return;

  }
  Serial.println("Checking server is up!");
  pingServer();
}

void loop() {

  fetchLightConfiguration();
  if (!client.connected()) {
    Serial.println("Client disconnected");
    client.stop();
  }
  delay(10000);
}

void pingServer() {

  Serial.println("Connecting to server at " + api);

  client.println("GET " + api + " HTTP/1.1");
  client.println("Connection: close");
  client.println();
  Serial.println("API called.");
  Serial.println(client);
  delay(10);
  while (client.connected()) {
    if (client.available()) {
      String c = client.readStringUntil('\0');
      Serial.print(c);
      Serial.println();
      break;
    }
  }

}

void fetchLightConfiguration() {

  if (client.connect(server, 3000)) {
    Serial.println("Connected to server at " + lightConfigApi);
    client.println("GET " + lightConfigApi + " HTTP/1.1" );
    client.println("Connection: close");
    client.println();
    Serial.println("Called " + lightConfigApi);

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

  Serial.println("Processing server response: ");
  Serial.println(response);
  DynamicJsonDocument doc(2048);

  DeserializationError error = deserializeJson(doc, response);
  if (error) {
    Serial.println("Deserialisation failed.");
    Serial.println(error.f_str());
    return;
  }
  Serial.println("Deserialise success...");
  JsonObject content = doc.as<JsonObject>();

  String name1 = content["name"].as<String>();
  Serial.println("Name: " + name1);

  JsonArray pixels = content["pixels"].as<JsonArray>();
  Serial.print("Pixel Array: ");
  Serial.println(pixels.size());
  for (JsonVariant pixel : pixels) {
    JsonObject led = pixel.as<JsonObject>();

    Serial.print("Red: ");
    Serial.println(led["red"].as<int>());

    Serial.print("Green: ");
    Serial.println(led["green"].as<int>());
  }


}

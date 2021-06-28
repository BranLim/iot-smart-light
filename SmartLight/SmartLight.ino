
#include <Adafruit_NeoPixel.h>
#include <WiFiNINA.h>

#define LED_PIN 2
#define LED_COUNT 8


Adafruit_NeoPixel stick(LED_COUNT, LED_PIN, NEO_RGB);
bool toggled = false;

uint8_t LEDColor[3] = {0, 0, 0};

const char *server = "192.168.10.143";
const char *ssid_name = "";
const char *ssid_pass = "";

const String api = "/api/v1/index";

WiFiClient client;
int wifiStatus = WL_IDLE_STATUS;

void setup() {

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
  if (client.connect(server, 3000)) {

    Serial.println("Connected to server");

    client.println("GET "+api+" HTTP/1.1");
    client.println("Connection: close");
    client.println();
    Serial.println("API called.");
    Serial.println(client);
  }
  else
  {
    Serial.println("Fail to connect to server!");
    return;
  }

  stick.begin();
  stick.show();
}

void loop() {

  while (client.available()) {
    char c = client.read();
    Serial.print(c);
  }
  Serial.println();
  if (!client.connected()) {
    Serial.println("Client disconnected");
    client.stop();

    for(;;);
  }


}


#include <Adafruit_NeoPixel.h>
#include <WiFiNINA.h>

#define LED_PIN 2
#define LED_COUNT 8


Adafruit_NeoPixel stick(LED_COUNT, LED_PIN, NEO_RGB);
bool toggled = false;

uint8_t LEDColor[3] = {0,0,0};

const char *server ="http://";
const char *ssid_name = "";
const char *ssid_pass = "";

const char *api = "/api/v1/";

WiFiSSLClient client;
int wifiStatus = WL_IDLE_STATUS;

void setup() {

  Serial.begin(9600);
  //while (!Serial);
  Serial.print("Initialising");
  Serial.println();

  while(wifiStatus != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid_name);  
    wifiStatus = WiFi.begin(ssid_name, ssid_pass);
    delay(10000);
  }
  Serial.println("WiFi connected");
  

  stick.begin();  
  stick.show();
}

void loop() {


}

#include <DNSServer.h>
#include <WiFiUdp.h>
#include <ESPAsyncTCP.h>
#include "ESPAsyncWebSrv.h"
#include <ESP8266WiFi.h>

#include <Adafruit_NeoPixel.h>

#include "arduino_secrets.h"
String ssid = AP_SSID;

// set the pin you'll be sending the digital signal on to your addressable LEDs
#define LED_PIN D4
// update this for your total LED count
#define LED_COUNT 22

DNSServer dnsServer;
AsyncWebServer server(80);

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// select your default colors
String top = "purple";
String bottom = "orange";

// color options can be added to top and bottom
// define the RGB in function handlePixelColor
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>Color Picker</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
  <h3>Select Colors</h3>
  <br><br>
  <form action="/set-color">
    <br>
    Top:
    <select name="top">
      <option value="red">Red</option>
      <option value="orange">Orange</option>
      <option value="yellow">Yellow</option>
      <option value="green">Green</option>
      <option value="teal">Teal</option>
      <option value="blue">Blue</option>
      <option value="purple">Purple</option>
      <option value="pink">Pink</option>
      <option value="white">White</option>
      <option value="black">Black</option>
    </select>
    <br>
    Bottom:
    <select name="bottom">
      <option value="red">Red</option>
      <option value="orange">Orange</option>
      <option value="yellow">Yellow</option>
      <option value="green">Green</option>
      <option value="blue">Blue</option>
      <option value="teal">Teal</option>
      <option value="purple">Purple</option>
      <option value="pink">Pink</option>
      <option value="white">White</option>
      <option value="black">Black</option>
    </select>
    <input type="submit" value="Submit">
  </form>
</body></html>)rawliteral";

class CaptiveRequestHandler : public AsyncWebHandler {
public:
  CaptiveRequestHandler() {}
  virtual ~CaptiveRequestHandler() {}

  bool canHandle(AsyncWebServerRequest *request){
    //request->addInterestingHeader("ANY");
    return true;
  }

  void handleRequest(AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html); 
  }
};

void setupServer(){
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html); 
    Serial.println("Client Connected");
  });
    
  server.on("/set-color", HTTP_GET, [] (AsyncWebServerRequest *request) {
    if (request->hasParam("top")) {
      top = request->getParam("top")->value();
    }

    if (request->hasParam("bottom")) {
      bottom = request->getParam("bottom")->value();
    }

    request->send(200, "text/html", "The values entered by you have been successfully sent to the device <br><a href=\"/\">Return to Home Page</a>");
  });
}

void handleStrip() {
  // first pixel will be our (possibly) unreliable level shifter from 3.3v to 5v
  // esp8266 GPIOs are 3.3v which can be unreliable without a level shifter
  // cover the first LED to prevent unexpected flickering
  strip.setPixelColor(0, strip.Color(0, 0, 0));

  // set all our base color (top)
  for (int i = 1; i < LED_COUNT; i++) {
    handlePixelColor(i, top);
  }

  // come back and set pixels 9 to 19 as bottom color
  for (int i = LED_BOTTOM_START; i <= LED_BOTTOM_END; i++) {
    handlePixelColor(i, bottom);
  }
  strip.show();
}

// you can add colors in the html as <option> in top and bottom, then define that color here
void handlePixelColor(int i, String color) {
  if (color == "red") {
    strip.setPixelColor(i, strip.Color(255, 0, 0));
  }
  else if (color == "orange") {
    strip.setPixelColor(i, strip.Color(255, 87, 0));
  }
  else if (color == "yellow") {
    strip.setPixelColor(i, strip.Color(255, 200, 0));
  }
  else if (color == "green") {
    strip.setPixelColor(i, strip.Color(0, 255, 0));
  }
  else if (color == "teal") {
    strip.setPixelColor(i, strip.Color(0, 255, 160));
  }
  else if (color == "blue") {
    strip.setPixelColor(i, strip.Color(0, 0, 255));
  }
  else if (color == "purple") {
    strip.setPixelColor(i, strip.Color(160, 0, 255));
  }
  else if (color == "pink") {
    strip.setPixelColor(i, strip.Color(255, 80, 255));
  }
  else if (color == "white") {
    strip.setPixelColor(i, strip.Color(255, 255, 150));
  }
  else if (color == "black") {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  }
  else {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  }
}

void setup(){
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.setBrightness(100);

  //your other setup stuff...
  Serial.begin(115200);
  Serial.println();
  Serial.println("Setting up AP Mode");
  WiFi.mode(WIFI_AP); 
  WiFi.softAP(ssid);
  Serial.print("AP IP address: ");Serial.println(WiFi.softAPIP());
  setupServer();

  Serial.println("Starting DNS Server");
  dnsServer.start(53, "*", WiFi.softAPIP());
  server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);//only when requested from AP
  //more handlers...
  server.begin();

  WiFi.setSleepMode(WIFI_NONE_SLEEP);
}

void loop(){
  dnsServer.processNextRequest();
  handleStrip();
}

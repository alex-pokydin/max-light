// esp8266 board: https://github.com/esp8266/Arduino#available-versions
//

#include <FS.h>
#include <time.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
typedef ESP8266WebServer  WiFiWebServer;
#include <AutoConnect.h>
#include "PageBuilder.h"

#include <Adafruit_NeoPixel.h>
#include <Ticker.h>
WiFiWebServer server;
AutoConnect portal(server);
AutoConnectConfig config;


long firstPixelHue = 0;

// NeoPixel
#define LED_PIN   4
#define LED_COUNT 80
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  
  Serial.begin(115200);
  Serial.println("Sarting...");
  Serial.print("Heap: "); Serial.println(ESP.getFreeHeap());
  
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.setBrightness(255); // Set BRIGHTNESS to about 1/5 (max = 255)
  strip.setPixelColor(0, strip.Color(0, 100, 255));
  strip.show();


  config.apid = "max-ap-" + String(ESP.getChipId(), HEX);
  config.psk = "1234567890";
  config.ota = AC_OTA_BUILTIN;
  config.title = "max lamp";
  config.hostName = "maxlamp-" + String(ESP.getChipId(), HEX);
  config.ticker = true;
  config.tickerPort = LED_BUILTIN;
  config.tickerOn = HIGH;
  config.boundaryOffset = 2000;
  config.autoReconnect = true;
//  config.portalTimeout = 60000;
//  config.immediateStart = true;
  portal.config(config);

  portal.begin();

}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    uint32_t colorProgress = strip.Color(255, 255, 0);
  }
  else {
    uint32_t colorProgress = strip.Color(0, 0, 255);
  }
  
  portal.handleClient();
  rainbow(10);             // Flowing rainbow cycle along the whole strip
  
  
}




// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait) {
  // Hue of first pixel runs 5 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this outer loop:
  
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    // Offset pixel hue by an amount to make one full revolution of the
    // color wheel (range of 65536) along the length of the strip
    // (strip.numPixels() steps):
    int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
    // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
    // optionally add saturation and value (brightness) (each 0 to 255).
    // Here we're using just the single-argument hue variant. The result
    // is passed through strip.gamma32() to provide 'truer' colors
    // before assigning to each pixel:
    strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    portal.handleClient();
  }
  strip.show(); // Update strip with new contents
  delay(wait);  // Pause for a moment
  firstPixelHue += 256; 
  if(firstPixelHue >= 5*65536){
    firstPixelHue = 0;
  }
  
}

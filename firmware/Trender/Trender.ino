#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Adafruit_NeoPixel.h>
#include <Ticker.h>

#define HOSTNAME "Trender"
#define mySSID "Trender"

ESP8266WebServer server(80);
Ticker tk;
volatile boolean showRainbow = false;
volatile uint32_t color = 0;

typedef struct t_config {
  uint8_t dur;              /**< total duration (min) */
  uint32_t colors[3];       /**< colors to display */
  uint8_t  per[2];          /**< threasholds */
} T_CONFIG;
volatile T_CONFIG config;

/*** timer variables ***/
unsigned long startTime = 0; /**< time of Trender timer start */
uint8_t step = 0; /**< current timer step 0-3 */

#include "Page_Color.h"
#include "Page_Config.h"
#include "Page_Admin.h"
#include "Page_Script_js.h"
#include "Page_Style_css.h"
#include "Page_Information.h"

#define PIN D2
#define NBPIX 3

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NBPIX, PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

void setup() {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  Serial.begin(115200);
  Serial.println();
  WiFi.hostname (HOSTNAME);
  Serial.println("Configuring access point...");
  WiFi.mode(WIFI_AP);
  WiFi.softAP(mySSID);
  Serial.println(WiFi.softAPIP());

  server.on ( "/", []() { Serial.println("/.html"); server.send ( 200, "text/html", PAGE_AdminMainPage );   }  );
  server.on ( "/admin", []() { Serial.println("admin.html"); server.send ( 200, "text/html", PAGE_AdminMainPage );   }  );
  server.on ( "/admin/infovalues", send_information_values_html);
  server.on ( "/info", []() { Serial.println("info.html"); server.send ( 200, "text/html", PAGE_Information );   }  );
//   server.on ( "/color", processColor);
//   server.on ( "/color/values", sendColorData);
  server.on ( "/config", processConfig);
  server.on ( "/config/values", sendConfigData);
  server.on ( "/style.css", []() { Serial.println("style.css"); server.send ( 200, "text/plain", PAGE_Style_css );  } );
  server.on ( "/microajax.js", []() { Serial.println("microajax.js"); server.send ( 200, "text/plain", PAGE_microajax_js );  } );
  server.on ( "/start", []() { Serial.println("start"); start(); server.send ( 200, "text/html", PAGE_AdminMainPage ); } );
  server.on ( "/stop", []() { Serial.println("stop"); stop(); server.send ( 200, "text/html", PAGE_AdminMainPage ); } );
  server.onNotFound ( []() { Serial.println("Page Not Found"); server.send ( 404, "text/html", "Page not Found" );   }  );

  // config init
  config.dur = 1;
  config.colors[0] = 0x00feff;
  config.colors[1] = 0xfffe00;
  config.colors[2] = 0xff0000;
  config.per[0] = 50;
  config.per[1] = 90;

  server.begin();
  Serial.println( "HTTP server started" );

  //tk.attach(0.05, tkColor);
}

void loop() {
  server.handleClient();
}

void tkColor() {
  uint16_t i;
  static uint16_t j=0;

  if (!showRainbow) {
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, color);
    }
  } else {
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(j));
    }
    j = (j+1) % 255;
  }
  strip.show();
}

/** set all pixel to color
 * @param col the color
 */
void setColor (uint32_t col) {
    uint8_t i=0;

    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, col);
    }
    strip.show();
}

void tkTrender() {
    unsigned long now = millis();

    if (now < (startTime + (config.dur*60000)*config.per[0]/100)) {
        setColor(config.colors[0]);
    } else
    if (now < (startTime + (config.dur*60000)*config.per[1]/100 )) {
        setColor(config.colors[1]);
    } else
    if (now < (startTime + (config.dur*60000))) {
        setColor(config.colors[2]);
    } else {
        stop();
    }
} // tkTrender

/** start timer
 */
void start (void) {
    step = 0;
    startTime = millis();
    Serial.println(startTime);
    setColor(config.colors[0]);
    delay(500);
    setColor(config.colors[1]);
    delay(500);
    setColor(config.colors[2]);
    delay(500);
    tk.attach(1, tkTrender);
} // start

/** stop timer
 */
void stop(void) {
    setColor(0);
    tk.detach();
}//stop

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}


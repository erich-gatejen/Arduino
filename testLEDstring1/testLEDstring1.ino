#include <FastLED.h>
#include <bitswap.h>
#include <chipsets.h>
#include <color.h>
#include <colorpalettes.h>
#include <colorutils.h>
#include <controller.h>
#include <cpp_compat.h>
#include <dmx.h>
#include <fastled_config.h>
#include <fastled_delay.h>
#include <fastled_progmem.h>
#include <fastpin.h>
#include <fastspi.h>
#include <fastspi_bitbang.h>
#include <fastspi_dma.h>
#include <fastspi_nop.h>
#include <fastspi_ref.h>
#include <fastspi_types.h>
#include <hsv2rgb.h>
#include <led_sysdefs.h>
#include <lib8tion.h>
#include <noise.h>
#include <pixelset.h>
#include <pixeltypes.h>
#include <platforms.h>
#include <power_mgt.h>

#define NUM_LEDS 50

CRGB leds[NUM_LEDS];

void setup() {
  FastLED.addLeds<WS2811, 5plxxx, RGB>(leds, NUM_LEDS);
  //.setCorrection(TypicalLEDStrip);
  //FastLED.setBrightness(150);
    pinMode(3, OUTPUT);
  digitalWrite(3, 255);
}

void loop() {
    leds[0] = CRGB::Red;
    leds[1] = CRGB::White;
    FastLED.show();
    analogWrite(3, 10);
    delay(2000);
    
    leds[0] = CRGB::White;
    leds[1] = CRGB::Red;
    FastLED.show();
    analogWrite(3, 250);
    delay(2000);

}




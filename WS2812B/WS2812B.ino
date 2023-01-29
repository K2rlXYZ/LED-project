#include <FastLED.h>

#define LED_PIN     10
#define NUM_LEDS    75
#define BRIGHTNESS  255
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

#define UPDATES_PER_SECOND 1000

void setup() {
  Serial.begin(38400);
  delay( 2000 ); // power-up safety delay
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(  BRIGHTNESS );
}

String str;
int red;
int green;
int blue;

void loop(){
  if (Serial.available() > 0) {
    if (Serial.read() == 'S'){
      blue  = Serial.parseInt();
      green = Serial.parseInt();
      red   = Serial.parseInt();
    }
    CRGB lastl;
    CRGB currentl;
    CRGB lastr;
    CRGB currentr;
    for(uint8_t x = 0; x <= NUM_LEDS/2-1; x++){
      if (x == 0) {
        currentl = leds[x];
        currentr = leds[x];
        leds[x]  = CRGB(green, red, blue);
        lastl    = currentl;
        lastr    = currentr;
      }
      else {
        currentl             = leds[NUM_LEDS/2 + x];
        currentr             = leds[NUM_LEDS/2 - x];
        leds[NUM_LEDS/2 + x] = lastl;
        leds[NUM_LEDS/2 - x] = lastr;
        lastl                = currentl;
        lastr                = currentr;
      }
    }
    
  }
  FastLED.show();
  /*for(uint8_t x = 0; x <= NUM_LEDS; x++){
      leds[x]  = CRGB(green, red, blue);
  }
  FastLED.show();
  delay(177);*/
}

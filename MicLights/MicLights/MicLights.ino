#include <arduinoFFT.h>

#include "FastLED.h"
#include <Servo.h>

#define NUM_LEDS 50
#define SAMPLES 128 // Must be a power of 2
#define SAMPLING_FREQUENCY 40000

arduinoFFT FFT = arduinoFFT();

unsigned int sampling_period_us;
unsigned long currentMicros, previousMicros;

double vReal[SAMPLES];
double vImag[SAMPLES];

CRGB leds[NUM_LEDS];
uint8_t gHue = 0; // rotating "base color"

void setup() {
  
  Serial.begin(9600);
  
  FastLED.addLeds<NEOPIXEL, A1>(leds, NUM_LEDS);
  pinMode(A0, INPUT);

  sampling_period_us = round(1000000 * (1.0 / SAMPLING_FREQUENCY));
}

double getDecibel() {
  for (int i = 0; i < SAMPLES; i++) {
    currentMicros = micros() - previousMicros; // To prevent the program from crashing when micros() overflows after ~70 min
    previousMicros = currentMicros;
    vReal[i] = analogRead(A0);
    vImag[i] = 0;
    while (micros() < (currentMicros + sampling_period_us)) {
      // do nothing
    }
  }
  FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
  FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);
  for (int i = 2; i < (SAMPLES/2); i++){
    if (vReal[i] > 100) {
      Serial.print(':');Serial.print(i);Serial.print(';');
      return i;
    }
  }
  return 0;
}

void loop() {
  int pot = analogRead(A0);
//  Serial.print(pot); Serial.println();

  fill_rainbow( leds, NUM_LEDS, gHue, 7);
  FastLED.show();
  FastLED.delay(50);


  int volume = map(getDecibel(), 0, 25, 2, 100);
  FastLED.setBrightness(volume);

  // Rotate through colors
  EVERY_N_MILLISECONDS( 1 ) { gHue = gHue + 10; }

  //  for (int i = 0; i < NUM_LEDS; i++) {
  //    leds[0] = CRGB::White; FastLED.show(); delay(30);
  //    leds[0] = CRGB::Black; FastLED.show(); delay(30);
  //  }

}

#include <FastLED.h>

#define LED_PIN     10
#define NUM_LEDS    75
#define BRIGHTNESS  255
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

#define UPDATES_PER_SECOND 1000

// This example shows several ways to set up and use 'palettes' of colors
// with FastLED.
//
// These compact palettes provide an easy way to re-colorize your
// animation on the fly, quickly, easily, and with low overhead.
//
// USING palettes is MUCH simpler in practice than in theory, so first just
// run this sketch, and watch the pretty lights as you then read through
// the code.  Although this sketch has eight (or more) different color schemes,
// the entire sketch compiles down to about 6.5K on AVR.
//
// FastLED provides a few pre-configured color palettes, and makes it
// extremely easy to make up your own color schemes with palettes.
//
// Some notes on the more abstract 'theory and practice' of
// FastLED compact palettes are at the bottom of this file.




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

/*
import numpy as np
import pyaudio
import pygame
import serial

import time
import sys

# Arduino related variables
PORT = 'COM3'
BAUD_RATE = 38400
DEVICE_NAME = ""
rcon = "0 if red < 110 else red*1.1"
gcon = "0 if red >= 110 else blue"
bcon = "0 if red >= 110 else green"

#(0 to x,multiplier1), (x to y,multiplier2), (y to 255,multiplier3)
rlist = [(100,1.1),(200,1.2),(255,1.4)]
glist = [(100,1.1),(200,1.2),(255,1.4)]
blist = [(100,1.1),(200,1.2),(255,1.4)]
speed = 2**11 # bigger power is slower

def codeR(lst, color):
    for tp in lst:
        if color <= tp[0]:
            color = color*tp[1]
def codeG(lst, color):
    for tp in lst:
        if color <= tp[0]:
            color = color*tp[1]
def codeB(lst, color):
    for tp in lst:
        if color <= tp[0]:
            color = color*tp[1]

# Initialize pyaudio and get WASAPI info
PyAudio = pyaudio.PyAudio()

device = {}
if DEVICE_NAME ==  "":
    device = PyAudio.get_device_info_by_index(PyAudio.get_host_api_info_by_type(pyaudio.paWASAPI)['defaultOutputDevice'])
else:
    for n in range(PyAudio.get_device_count()):
        if (PyAudio.get_device_info_by_index(n)['hostApi'] == WASAPI_host_index) and \
        (DEVICE_NAME.lower() == PyAudio.get_device_info_by_index(n)['name'].lower()):
            device = PyAudio.get_device_info_by_index(n)

ser = serial.Serial(PORT, BAUD_RATE, timeout=0.1)

# Get default WASAPI output if available else exit

# Get appropriate info about the default WASAPI output device
DEVICE_INDEX = device['index']
RATE = int(device['defaultSampleRate'])
CHANNELS = device['maxOutputChannels']
FORMAT = pyaudio.paInt16
BUFFER = round(speed)
MIN_FREQ_BAND = RATE / BUFFER
FOURIER_LEN = int(BUFFER / 2)
#screen = pygame.display.set_mode((480, 480))


def get_fourier_from_stream(stream):
    data = np.frombuffer(stream, "int16")
    fourier = np.abs(np.fft.rfft(data[::2]))
    fourier[0] = 0
    avg_volume = 10 * np.log10(np.mean(fourier))
    return fourier, avg_volume


def get_bands(fourier):

    bass_band_limit = 300
    midrange_limit = 900
    upper_limit = 3000

    bass_band = fourier[1:int(bass_band_limit / MIN_FREQ_BAND)]
    midrange_band = fourier[int(bass_band_limit / MIN_FREQ_BAND):int(midrange_limit / MIN_FREQ_BAND)]
    upper_band = fourier[int(midrange_limit / MIN_FREQ_BAND):int(upper_limit / MIN_FREQ_BAND):]

    return bass_band, midrange_band, upper_band


def get_bands_mean(bands):
    return [np.mean(band) for band in bands]


def get_bands_mean_db(bands_mean):
    return [10 * (np.log10(band_mean) if band_mean != 0 else 0) for band_mean in bands_mean]


def get_bands_rgb(bands_means, avg_volume):
    r, g, b = 0, 0, 0
    max_amplitude = 57.5
    min_amplitude = 30
    if avg_volume > min_amplitude:
        r, b, g = get_bands_mean_db(bands_means)
        #print('\r', f"{r},{g},{b}", end="")

        r = r - min_amplitude
        g = g - min_amplitude
        b = b - min_amplitude


        r = round(int(255 * (r / max_amplitude)))
        b = round(int(255 * (b / max_amplitude)))
        g = round(int(255 * (g / max_amplitude)))


        #print('\r', f"{r},{g},{b}", end="")
        r = float(eval(rcon.replace("red", str(r)).replace("green", str(g)).replace("blue", str(b))))
        g = float(eval(gcon.replace("red", str(r)).replace("green", str(g)).replace("blue", str(b))))
        b = float(eval(bcon.replace("red", str(r)).replace("green", str(g)).replace("blue", str(b))))

        codeR(rlist,r)
        codeG(glist,g)
        codeB(blist,b)

        r,g,b = round(r),round(g),round(b)
        r, g, b = valid_rgb(r, g, b)

    return r, g, b


def get_rgb(data):
    # red part / bass part
    bass_band = data[1: round(250 / MIN_FREQ_BAND)]
    peak_bass = np.argmax(bass_band) + 1
    r = int(peak_bass * 255 / len(bass_band))

    medium_band = data[round(250 / MIN_FREQ_BAND): round(2000 / MIN_FREQ_BAND)]
    peak_medium = np.argmax(medium_band) + 1
    g = int(peak_medium * 255 / len(medium_band))

    high_band = data[round(2000 / MIN_FREQ_BAND): round(6000 / MIN_FREQ_BAND)]
    peak_high = np.argmax(high_band) + 1
    b = int(peak_high * 255 / len(high_band))

    return r, g, b


def valid_rgb(r, g, b):
    r = max(0, r)
    g = max(0, g)
    b = max(0, b)

    r = min(255, r)
    g = min(255, g)
    b = min(255, b)

    return r, g, b

def retN(n: int):
    if n > 99:
        return str(n)
    elif n > 9:
        return "0" + str(n)
    else:
        return "00" + str(n)

def callback(in_stream, frame_count, time_info, status):
    fourier, avg_volume = get_fourier_from_stream(in_stream)
    fourier = np.where(fourier > avg_volume, fourier, 0)
    # r, g, b = get_rgb(fourier)
    bands = get_bands(fourier)
    band_means = get_bands_mean(bands)
    r, g, b = get_bands_rgb(band_means, avg_volume)
    rs,gs,bs = retN(round(r)),retN(round(g)),retN(round(b))
    #print(f"\rS{g}i{b}i{r}", end="")
    ser.write(str.encode(f"S{gs}r{rs}b{bs}S"))
    #print('\r', f"S{gs}r{rs}b{bs}S", end="")
    #print('\r', r, g, b, end="")
    #screen.fill((r, g, b))
    #pygame.display.flip()


    return in_stream, pyaudio.paContinue

STREAM = PyAudio.open(input_device_index=device["index"],
                      format=FORMAT,
                      channels=CHANNELS,
                      rate=RATE,
                      input=True,
                      frames_per_buffer=BUFFER,
                      stream_callback=callback,
                      as_loopback=True)

STREAM.start_stream()

print("Starting recording...")

while STREAM.is_active():
    pass
    #for event in pygame.event.get():
        #continue

STREAM.stop_stream()
STREAM.close()
PyAudio.terminate()
*/

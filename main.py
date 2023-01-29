import numpy as np
import pyaudio
import pygame
import serial

import warnings
warnings.filterwarnings("ignore")

import time
import sys

"""(width, height) = (300, 200)
screen = pygame.display.set_mode((width, height))
pygame.display.flip()
pygame.font.init()
myfont = pygame.font.SysFont('Comic Sans MS', 30)"""

# Arduino related variables
PORT = 'COM3'
BAUD_RATE = 38400
DEVICE_NAME = ""

max_amplitude = 57.5
min_amplitude = 15

rcon = "red if red < 100 else red*1.7"
gcon = "0"#"255 if blue >= 105 else 0"
bcon = "0 if blue > 140 else blue"#"blue if blue < 105 else 255"

#(0 to x,multiplier1), (x to y,multiplier2), (y to 255,multiplier3)
rlist = [(50,1.2),(100,1.4),(150,1.6),(200,1.8),(255,2)]
glist = [(50,0.1),(100,0.2),(150,0.4),(200,0.6),(255,0.8)]
blist = [(50,0.2),(100,0.4),(150,0.7),(200,0.9),(255,1)]


def code(lst, color):
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
        if DEVICE_NAME.lower() in PyAudio.get_device_info_by_index(n)['name'].lower():
            device = PyAudio.get_device_info_by_index(n)

ser = serial.Serial(PORT, BAUD_RATE, timeout=0.1)

# Get default WASAPI output if available else exit

# Get appropriate info about the default WASAPI output device
DEVICE_INDEX = device['index']
RATE = int(device['defaultSampleRate'])
CHANNELS = device['maxOutputChannels']
FORMAT = pyaudio.paInt16
SPEEDMULT = 10.5
SPEED = 2 ** SPEEDMULT # bigger power is slower
BUFFER = round(SPEED)
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

    bass_band_limit = 90
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
    
    if avg_volume > min_amplitude:
        b, g, r = get_bands_mean_db(bands_means)
        #print('\r', f"{r},{g},{b}", end="")

        r = r - min_amplitude
        g = g - min_amplitude
        b = b - min_amplitude


        r = round(int(255 * (r / max_amplitude)))
        b = round(int(255 * (b / max_amplitude)))
        g = round(int(255 * (g / max_amplitude)))

        #code(rlist, r);code(glist, g);code(blist, b)

        if rcon != "":
            r = float(eval(rcon.replace("red", str(r)).replace("green", str(g)).replace("blue", str(b))))
        if gcon != "":
            g = float(eval(gcon.replace("red", str(r)).replace("green", str(g)).replace("blue", str(b))))
        if bcon != "":
            b = float(eval(bcon.replace("red", str(r)).replace("green", str(g)).replace("blue", str(b))))
        #print('\r', f"{r},{g},{b}", end="")


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

def callback(in_stream, frame_count, time_info, status, speedmult=SPEEDMULT):
    fourier, avg_volume = get_fourier_from_stream(in_stream)
    fourier = np.where(fourier > avg_volume, fourier, 0)
    # r, g, b = get_rgb(fourier)
    bands = get_bands(fourier)
    band_means = get_bands_mean(bands)
    r, g, b = get_bands_rgb(band_means, avg_volume)
    rs,gs,bs = retN(round(r)),retN(round(g)),retN(round(b))
    #print(f"\rS{g}i{b}i{r}", end="")
    #b,g,r
    ser.write(str.encode(f"S{bs}r{gs}b{rs}S"))
    #print('\r', f"S{gs}r{rs}b{bs}S", end="")
    print('\r', r, g, b, speedmult, "      ", end="")

    """screen.fill((r, g, b))
    textsurface = myfont.render(f"{r}   {g}   {b}", False, (abs(r - 255), abs(g - 255), abs(b - 255)))
    textsurface1 = myfont.render(f"{speedmult}", False, (abs(r - 255), abs(g - 255), abs(b - 255)))
    screen.blit(textsurface, (0, 0))
    screen.blit(textsurface1, (0, 40))
    pygame.display.flip()"""


    return in_stream, pyaudio.paContinue

STREAM = PyAudio.open(input_device_index=device['index'],
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

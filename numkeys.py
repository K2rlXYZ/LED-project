import serial
import time
import pygame
import sys



PORT = 'COM3'
BAUD_RATE = 38400
r,g,b = 0,255,255

(width, height) = (300, 200)
screen = pygame.display.set_mode((width, height))
pygame.display.flip()
pygame.font.init()
myfont = pygame.font.SysFont('Comic Sans MS', 30)

ser = serial.Serial(PORT, BAUD_RATE, timeout=0.1)

def checkColor(color):
    color = 255 if color > 255 else color
    color = 0 if color < 0 else color
    return color

down =  False
act = ""
last = 0
while 1:
    events = pygame.event.get()
    if down and time.perf_counter()-last > 0.08:
        last=time.perf_counter()
        if act == "+r":
            r+=10
        if act == "+g":
            g+=10
        if act == "+b":
            b+=10
        if act == "-r":
            r-=10
        if act == "-g":
            g-=10
        if act == "-b":
            b-=10
    for event in events:
        if event.type == pygame.KEYDOWN:
            down=True
            if event.key == pygame.K_KP7:
                r+=2
                act="+r"
            if event.key == pygame.K_KP8:
                g+=2
                act = "+g"
            if event.key == pygame.K_KP9:
                b+=2
                act = "+b"
            if event.key == pygame.K_KP4:
                r-=2
                act = "-r"
            if event.key == pygame.K_KP5:
                g-=2
                act = "-g"
            if event.key == pygame.K_KP6:
                b-=2
                act = "-b"
            if event.key == pygame.K_KP1:
                r,g,b = 0,0,0
            if event.key == pygame.K_KP2:
                r,g,b = 50,50,50
            if event.key == pygame.K_KP3:
                r,g,b = 255,255,255
            if event.key == pygame.K_l:
                r,g,b = 255,30,0
            if event.key == pygame.K_p:
                r,g,b = 255,0,255
        elif event.type == pygame.KEYUP:
            down=False
            act=""
        if event.type == pygame.QUIT:
            pygame.quit()
            exit()
    r,g,b=checkColor(r),checkColor(g),checkColor(b)
    screen.fill((r,g,b))
    textsurface = myfont.render(f"{r}   {g}   {b}", False, (abs(r-255), abs(g-255), abs(b-255)))
    screen.blit(textsurface, (0, 0))
    pygame.display.flip()
    time.sleep(0.05)
    line = ser.readline()
    print(line)
    ser.write(str.encode(f"S{b}r{g}g{r}S"))






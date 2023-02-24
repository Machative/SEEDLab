# I2C
import smbus
import time
# LCD Screen
import board
import busio
import adafruit_character_lcd.character_lcd_rgb_i2c as character_lcd

import math
#Imports numpy and opencv
import numpy as np
import cv2 as cv

#Imports picamera
from picamera import PiCamera
from time import sleep

#Initializes the camera and the video capture object
cap = cv.VideoCapture(0)

position = [0]

lcd_columns = 16
lcd_rows = 2

# Initialise I2C bus.
i2c = board.I2C()  # uses board.SCL and board.SDA
# i2c = board.STEMMA_I2C()  # For using the built-in STEMMA QT connector on a microcontroller

# Initialise the LCD class
lcd = character_lcd.Character_LCD_RGB_I2C(i2c, lcd_columns, lcd_rows)

lcd.clear()
lcd.color = [50, 0, 50]
# for RPI version 1, use “bus = smbus.SMBus(0)”
bus = smbus.SMBus(1)

# This is the address we setup in the Arduino Program
address = 0x04

position = [0];
def writeNumber(value):
    # We're sending multiple bytes, so we use this I2C block operation
    bus.write_i2c_block_data(address, 0, value)
    #bus.write_byte(address, value)
    #bus.write_byte_data(address, offset, value)
    return -1

def readNumber(length):
    # When reading in the list from the Arduino, I have length+1 to account for the
    # first element being 0, and then use block.pop() to remove that 0.
    block = bus.read_i2c_block_data(address, 0, 2)
    #block.pop()
    #block = bus.read_byte_data(address, 0xFF)
    return block

#Captures each frame of the video
while cap.isOpened():
    x, getFrame = cap.read()
    if not x:
        print('Didnt get the frame. Ending the operation')
        break
    #Defines the dictionary of aruco types we will be searching for
    dictionary = cv.aruco.getPredefinedDictionary(cv.aruco.DICT_6X6_1000)
    #Defines the parameters we will use when detecting for markers
    arguments = cv.aruco.DetectorParameters_create()
    #Detects for markers and finds corners, ids and if there are any rejected markers
    (corners, ids, rejectedImgPoints) = cv.aruco.detectMarkers(getFrame, dictionary, parameters = arguments)
    if ids is None:
        print('No markers found')
    else:
        #1920x1080 imgage with a horizontal view of 66 degrees
        horizontalPixels = getFrame.shape[1]
        verticalPixels = getFrame.shape[0]
        horizontalFOV = 66
        halfView = horizontalFOV / 2
        #Getting the approximate pixel locations of the marker
        marker = corners[0]
        #print(marker)
        #Gets the x and y coordinates of the top left corner
        topLeft = marker[0,0]
        x1 = topLeft[0]
        y1 = topLeft[1]
        #Gets the x coordinate of the top right corner
        topRight = marker[0,1]
        x2 = topRight[0]
        y2 = topRight[1]
        #Gets the x coordinate of the bottom right corner
        botRight = marker[0,2]
        x3 = botRight[0]
        y3 = botRight[1]
        #Gets the x coordinate of the bottom left corner
        botLeft = marker[0,3]
        x4 = botLeft[0]
        y4 = botLeft[1]
        #Approximates pixel location based on corner locations
        xCenter = (x1+x2+x3+x4)/4
        yCenter = (y1+y2+y3+y4)/4
        #Prints the quadrant of the marker
        #If marker in top left 
        if((xCenter < (horizontalPixels / 2)) and (yCenter < (verticalPixels / 2))):
            position[0] = 2
            writeNumber(position)
            time.sleep(.1)
            number = readNumber()
            time.sleep(.1)
            message = str(number[0]) + str(number[1]) + str(number[2])
            print('In top left corner')
            print(message)
            
            lcd.text_direction = lcd.LEFT_TO_RIGHT
            lcd.message = "Desired: Quad " + str(position) + "\nCurrent: " + message
        elif((xCenter > (horizontalPixels / 2)) and (yCenter < (verticalPixels / 2))):
            position[0] = 1
            writeNumber(position)
            time.sleep(.1)
            number = readNumber()
            time.sleep(.1)
            message = str(number[0]) + str(number[1]) + str(number[2])
            print('In top right corner')
            print(message)
            
            lcd.text_direction = lcd.LEFT_TO_RIGHT
            lcd.message = "Desired: Quad " + str(position) + "\nCurrent: " + message
        elif((xCenter < (horizontalPixels / 2)) and (yCenter > (verticalPixels / 2))):
            position[0] = 3
            writeNumber(position)
            time.sleep(.1)
            number = readNumber()
            time.sleep(.1)
            message = str(number[0]) + str(number[1]) + str(number[2])
            print('In bottom left corner')
            print(message)
            
            lcd.text_direction = lcd.LEFT_TO_RIGHT
            lcd.message = "Desired: Quad " + str(position) + "\nCurrent: " + message
        elif((xCenter > (horizontalPixels / 2)) and (yCenter > (verticalPixels / 2))):
             position[0] = 4
            writeNumber(position)
            time.sleep(.1)
            number = readNumber()
            time.sleep(.1)
            message = str(number[0]) + str(number[1]) + str(number[2])
            print('In bottom right corner')
            print(message)
            
            lcd.text_direction = lcd.LEFT_TO_RIGHT
            lcd.message = "Desired: Quad " + str(position) + "\nCurrent: " + message
            
   

    drawnImg = cv.aruco.drawDetectedMarkers(getFrame, corners)
    #output.write(getFrame)
    cv.imshow('Frame', drawnImg)
    
    time.sleep(.01)
    # take in dumby value from I2C
    number = readNumber()
    currentPosition = number[0] + number[1]
    #Press q to stop recording
    if cv.waitKey(1) == ord('q'):
        break
#When done taking the video
cap.release()
#output.release()
cv.destroyAllWindows()

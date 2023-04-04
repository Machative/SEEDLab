#Imports numpy and opencv
import numpy as np
import cv2 as cv

#I2C
import smbus
import time
# LCD Screen
import board
import busio
import adafruit_character_lcd.character_lcd_rgb_i2c as character_lcd

#Imports picamera
from picamera import PiCamera
from time import sleep

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

#Initializes the camera and the video capture object
cap = cv.VideoCapture(0)

#Convert to lower resolution (half of 1920x1080) to allow for faster detection
#PiCamera.resolution = (960,540)
address = 0x04
data = [0,0]
height [0,0]
def writeNumber(data):
        bus.write_i2C_block_data(address,0,data)
        return -1

def writeHeight(height):
        bus.write_byte(address, height)
def readNumber():
        number = bus.read_byte(address)
        return number

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
        #print (horizontalPixels)
        #print(verticalPixels)
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
        print(xCenter)
        #Calculates the angle
        xPixFromCenter = (horizontalPixels/2) - xCenter
        angle = halfView*(xPixFromCenter/(horizontalPixels/2))
        print('The first marker is ', angle, 'degrees away from the center of the image')
        
        intAngle = int(angle)
        decAngle = angle - decAngle
        decAngle *= 10
        decAngle = int(decAngle)
        
        data[0] = intAngle
        data[1] = decAngle
        time.sleep(.1)
        writeNumber(data)
        print(data)
        
     
        #Approximates pixel location based on corner locations
        xCenter = (x1+x2+x3+x4)/4
        yCenter = (y1+y2+y3+y4)/4
        yTopCenter = (y1+y2)/2
        yBotCenter = (y3+y4)/2
        markerHeight = yTopCenter-yBotCenter
        print(markerHeight)
        #Tells robot to stop when marker is within a foot
        #### TYPE CODE HERE ####
        
        status = readNumber()
        if (status == 1):
                writeHeight(markerHeight)
        
        #If marker height is greater than 100 pixels, then stop the robot from moving forward. (Within 1 foot)
        
        # Print the angle to the LCD screen
        #time.sleep(.1)
        #lcd.text_direction = lcd.LEFT_TO_RIGHT
        #lcd.message = "Angle: " + str(angle)
        
        
        
    drawnImg = cv.aruco.drawDetectedMarkers(getFrame, corners)
    #output.write(getFrame)
    cv.imshow('Frame', drawnImg)
    #Press q to stop recording
    if cv.waitKey(1) == ord('q'):
        break
#When done taking the video
cap.release()
#output.release()
cv.destroyAllWindows()

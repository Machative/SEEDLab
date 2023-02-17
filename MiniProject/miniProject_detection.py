#Imports numpy and opencv
import numpy as np
import cv2 as cv

#Imports picamera
from picamera import PiCamera
from time import sleep

#Initializes the camera and the video capture object
cap = cv.VideoCapture(0)

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
            print('In top left corner')
        elif((xCenter > (horizontalPixels / 2)) and (yCenter < (verticalPixels / 2))):
            print('In top right corner')
        elif((xCenter < (horizontalPixels / 2)) and (yCenter > (verticalPixels / 2))):
            print('In bottom left corner')
        elif((xCenter > (horizontalPixels / 2)) and (yCenter > (verticalPixels / 2))):
            print('In bottom right corner')
    
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
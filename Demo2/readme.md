This folder contains all the code for Demo 2. The purpose of Demo 2 is to rotate in a circle until a marker is detected, then to drive forward until the robot is within a foot of the marker.

The file arduino.ino is used to...

The file demo2_detection.py is used to first turn on the camera and start looking for markers. Once a marker is detected, the camera sends the angle to the arduino. Then once the Pi receives indication that the robot has been straightened out, the arduino then tells the Pi to send the height in pixels of the detected aruco marker. Using the pixel locations of the 4 corners of the detected marker, the average pixel height is found for the aruco marker. Then the Pi sends the pixel height to the arduino, which uses the height to calculate the distance that the robot is away from the marker.

The file distance_calculation.xlsx is used as a method of approximating the distance the marker is away from the robot based off of data accumulated. We first took measurements of the pixel height of the marker at different distances. Then, we plugged those values into an excel spreadsheet to then graph and develop an equation to approximate the distance that the robot is away from the marker. We then used this equation in the arduino code to tell the robot how far forward to drive based off of the pixel heights read by the camera.

This folder contains all the code for Demo 2. The purpose of Demo 2 is to rotate in a circle until a marker is detected, then to drive forward until the robot is within a foot of the marker.

The file arduino.ino is used to...

The file demo2_detection is used to...

The file distance_calculation.xlsx is used as a method of approximating the distance the marker is away based off of data accumulated. We first took measurements of the pixel height of the markers at different distances. Then, we plugged those values into an excel spreadsheet to then graph and develop an equation to approximate the distance that the robot is away from the marker. We then used this equation in the arduino code to tell the robot how far forward to drive based off of the pixel heights read by the camera.

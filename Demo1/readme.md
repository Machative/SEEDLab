This folder contains all the code for Demo 1.
The purpose of Demo 1 is threefold. The 3 tasks are listed below.
1. Detect an Aruco marker and display the angle between the robot and the marker on an LCD.
2. Drive a fixed straight-line distance.
3. Turn a fixed angle and then drive a fixed straight-line distance.

The file arduinoControlOld.ino was the original attempt at the control system for the robot to complete Tasks 1 and 2. This control system made use of an x,y,phi coordinate system and the desired angle and distance were interpreted as such. It was determined that this system was not ideal and brought up too many issues that complicated the problem, so it was abandoned for a simpler scheme for the purpose of this Demo. This scheme is described below.

The file robotTunedExp.ino contains the final control system for the robot. Instead of using an x,y,phi system, this control system simply separates the turning and driving into two explicit controlled motions. The turning is executed first in its own control loop with its own PID values, and then the straight-line driving is executed second in its own control loop. The actions are treated as completely separate, in contrast with the original control scheme which tried to combine them and thus overcomplicated the problem.

The file demo1_detection.py __

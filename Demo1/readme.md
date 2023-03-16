This folder contains all the code for Demo 1.
The purpose of Demo 1 is threefold. The 3 tasks are listed below.
1. Detect an Aruco marker and display the angle between the robot and the marker on an LCD.
2. Drive a fixed straight-line distance.
3. Turn a fixed angle and then drive a fixed straight-line distance.

The file arduinoControlOld.ino was the original attempt at the control system for the robot to complete Tasks 1 and 2. This control system made use of an x,y,phi coordinate system and the desired angle and distance were interpreted as such. It was determined that this system was not ideal and brought up too many issues that complicated the problem, so it was abandoned for a simpler scheme for the purpose of this Demo. This scheme is described below.

The file robotTunedExp.ino contains the final control system for the robot. Instead of using an x,y,phi system, this control system simply separates the turning and driving into two explicit controlled motions. The turning is executed first in its own control loop with its own PID values, and then the straight-line driving is executed second in its own control loop. The actions are treated as completely separate, in contrast with the original control scheme which tried to combine them and thus overcomplicated the problem.

The file demo1_detection.py opens up a stream of camera frames, detects if any markers are present in the frame. If there are any markers present, it gets the pixel locations of each of the corners of the marker. It then uses the corner locations to find the center of the marker. Since the field of view of the camera is known, we can use it along side the markers location to calculate the horizontal angle at which the marker is away from the center of the frame. Then, we send this angle to the LCD display on the robot.

The file recordRobotRot.ino is used to record the rotation of the wheels on the robot, essentially verifying that the encoders are wokring in the way that we wish for them to. It does this by creating encoder objects that are supposedly fed in the respective motor pins they need, and then these encoder objects rotation is read in a loop. This allows one to know if the wiring is correct, as if it is, then a rotation on the wheel should register on the loop, while if it does not, something went wrong.

The file robotIdentificationExp.ino contains the program used to implement a step response for moving straight ahead and turning on the robot. It executes a step response on the voltage applied to the motors neccessary for the desired experiment and then stops after ~3 s. The data of each motors' displacement is fed in, and processed in order to find the velocity forwards and the rate of rotation of the robot, which is then printed out via Serial monitor to be collected and processed for simulation purposes.

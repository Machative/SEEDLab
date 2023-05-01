MATLAB & Simulation Files README
DESCRIPTIONS OF MOST IF NOT ALL CONSTITUENT FILES BELOW.

-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

NOTE THIS DESCRIPTION IS PARTIALLY COPIED OF THE ONE FOR DEMO 2. NOT MUCH/ANYTHING CHANGED IN MATLAB AND SIMULATION LAND, HENCE THE FEW CHANGES.
This folder contains work done to simulate the robot. 
Note, the control systems developed here are not in use in the Arduino. Why?
Well, not neccessarily in order, a couple reasons.

-   From precedent and judgement: we didn't really use simulation and control results in Demo 1 or Demo 2, so we didn't use simulation here either much/at all. It also didn't seem like the simulation and control task was to prove more useful in the Final Demo, and from what I can judge, it doesn't seem like it would have been especially useful here even if we had better committed to using it more as intended. In fact, it might have caused additional costs and issues, including extra work on my part of Simulation and Control. This is because we might have still needed to tune the control system /other robot code as we tested the robot, and any changes proposed on the Arduino could have caused downstream impacts requiring remodelling, resimulating, and retuning in MATLAB and Simulink if we were to rigidly go from MATLAB and Simulink model implemented into the Arduino. Other costs may have been greater time pressure for the project on both me and the rest of the team potentially as I need to get control results done by a deadline, and in turn, the Arduino control scheme has to wait on simulation results. 

-   From personal/other reasons: I might have not wanted to put the time and effort that ultimately does not seem like it would have made an especially critical impact. Other reasons possible as well. 

-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

So why is there anything here at all? Well, DEMO 2. A lot of this is from Demo 2.

-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

With all that said, what is here? Well, there are: 
-   Excel measurement files of the robot's step response to a signal that is the same magnitude across both wheels but either the same or different in polarity corresponding to forward and turning respectively. These are "ID_velExperiment.xlsx" and "ID_angVelExperiment.xlsx" respectively.
-   MATLAB.m files that pull the corresponding Excel sheet and Simulink transfer function model of the robot for a step response in both angular and forward directions, which were used "demo1_avgVa_Identification.m" and "demo1_delVa_Identification.m"
-   A simulink model used in the MATLAB .m files above, "RobotStepID.slx" and "RobotStepID.slxc".
All those files generally originate for Demo 1, but were used in Demo 2. Specifically, the transfer function described by the two .m files of the robot's forward and angular characteristics was converted to a discrete transfer function in "robot_continuous_vars_workspace.m".
-   As mentioned, "robot_continuous_vars_workspace.m" converts the continuous transfer function found for the robot in continuous time to discrete time transfer functions with sampling time of 0.02 seconds (20 ms) using a ZOH discretization method (the dafult for c2d in MATLAB). It outputs 2 .mat files that store data about the continuous and discrete transfer functions. 
-   The two .mat files, "robotContVars.mat" and "robotDiscVars.mat" storing data for the continuous and discrete transfer functions respectively.
-   "controlVarScript.m" loads the two .mat files and has some other variables it creates, Simulink models draw on a subset (not neccessarily a proper subset if I recall correctly) of what it initializes. 
-   "RobotCotrolEnsemble1.slx" an "RobotCotrolEnsemble1.slx" is the Simulink model implementing an outer feedback loop with a discrete PD controller for displacement and an inner feedback loop with a discrete P controller for velocity for both the velocity and angular discrete transfer functions. Both outer PD controllers use a filtered derivative with a filter coefficient of 1, as this was found to greatly reduce issues. See the document, "Robot Control Ensemble Results.docx" for pictures and elaboration on this topic. 
-   "Robot Control Ensemble Results.docx" expands upon what the "RobotControlEnsemble1.slx" file does, and places pictures of the results of the 2 systems to a step function input of varying magnitude to demonstrate the adequacy of the controllers. 


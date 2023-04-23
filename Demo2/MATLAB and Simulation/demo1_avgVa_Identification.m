%% Set-up
close all
clear
clc

%% Define motor experiment and transfer function parameters before running/loading
% Simulation
kV = 0.51*12.5;
om_nV = 1*12.5;
setaV = 1.33;

kPhi = 0*3.3*12.5;
om_nPhi = 1*12.5;
setaPhi = 1.55;

%% Get data from experiment and simulation
% Motor experiment file with data
robotResp = readmatrix('ID_velExperiment.xlsx');

% Simulation
open_system('RobotStepID');
out = sim('RobotStepID');

%% Define motor experiment and transfer function parameters after running/loading
% Motor experiment parameters, time converted to [s] from [ms]
motLen = size(robotResp, 1);
v = robotResp(:,2);
om = robotResp(:,3);
t = robotResp(:,1) ./ 1000;

% Period used in experiment
delT = t(4) - t(3);

% Get displacement from velocity for experiment
x = zeros(motLen, 1);
x(1) = 0;
for i1 = 2:motLen
    x(i1) = x(i1-1) + (v(i1)+v(i1-1))*delT/2;
end

phi = zeros(motLen, 1);
phi(1) = 0;
for i1 = 2:motLen
    phi(i1) = phi(i1-1) + (om(i1)+om(i1-1))*delT/2;
end

% Filtering used to smooth the expeirment data
vFilt = smoothdata(v,'gaussian',5);
omFilt = smoothdata(om,'gaussian',5);

vAvg = smoothdata(v,'movmean',100);
omAvg = smoothdata(om,'movmean',100);

%% Output graphics
% Legends are missing this: "Transfer Function",
% Displacement for both experiment and simulation
figure
plot(t, x);
hold on

plot(out.get('disp'));

% Formatting
title("Displacement of a Robot for a Voltage Step Function Applied");
xlabel("Time");
ylabel("Displacement")
legend("Robot Step Response","Transfer Function","Location","southoutside")
hold off

% Velocity for both experiment and simulation
figure
plot(t, vFilt);
hold on

plot(out.velocity);

% Formatting
title("Velocity of a Robot for a Voltage Step Function Applied");
xlabel("Time");
ylabel("Velocity")
legend("Robot Step Response","Transfer Function","Location","southoutside")
hold off

% Angular displacement for both experiment and simulation
figure
plot(t, phi);
hold on

plot(out.get('angDisp'));

% Formatting
title("Angular Displacement of a Robot for a Voltage Step Function Applied");
xlabel("Time");
ylabel("Angular Displacement")
legend("Robot Step Response","Transfer Function","Location","southoutside")
hold off

% Angular velocity for both experiment and simulation
figure
plot(t, omFilt);
hold on

plot(out.angVelocity);

% Formatting
title("Angular Velocity of a Robot for a Voltage Step Function Applied");
xlabel("Time");
ylabel("Angular Velocity")
legend("Robot Step Response","Transfer Function","Location","southoutside")
hold off

%% Discussion of Results
% It can be seen that the robot moves for about _ [s], and moves about
% _ [m]. The robot was measured to move 1.50 to 1.53 [m] in about 3.05 [s], and the
% fact that these values for time and velocity are about equal with human
% error involved suggest that the calculations performed in the Arduino to
% process the encoders and the program in MATLAB are correct, as the values
% seem close enough.
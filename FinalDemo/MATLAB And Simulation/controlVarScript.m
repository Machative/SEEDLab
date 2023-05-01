close all
clear
clc

load('robotDiscVars.mat')
load('robotContVars.mat')

cRate = 0.02;
arduinoMaxSpeed = 65; % In bits out of 0 to 255 bits
speedLim = 8 * arduinoMaxSpeed / 255; % In volts
PWM_conversion = 8 / 255;
robotDZ = 0.1;
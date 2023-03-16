%% Set-up
close all
clear
clc

%% Define transfer function parameters before running/loading simulation
% Simulation parameters used in Robot Control 1, are equivalent to
% parameters found in avgVa and delVa MATLAB files
kV = 0.51*12.5;
om_nV = 1*12.5;
setaV = 1.33;

kPhi = 3.3*12.5;
om_nPhi = 1*12.5;
setaPhi = 1.55;

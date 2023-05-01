close all
clear
clc

kV = 0.51*12.5;
om_nV = 1*12.5;
setaV = 1.33;

kPhi = 3.3*12.5;
om_nPhi = 1*12.5;
setaPhi = 1.55;

save("robotContVars");

s = tf('s');

velTF = (kV*s + kV*om_nV) / (s^2 + 2*setaV*om_nV*s + om_nV^2*s);
angTF = (kPhi*s + kPhi*om_nV) / (s^2 + 2*setaPhi*om_nPhi*s + om_nPhi^2*s);

velTF_d = c2d(velTF, 0.02);
angTF_d = c2d(angTF, 0.02);

velTF_dNum = cell2mat(velTF_d.Numerator);
velTF_dDen = cell2mat(velTF_d.Denominator);

angTF_dNum = cell2mat(angTF_d.Numerator);
angTF_dDen = cell2mat(angTF_d.Denominator);

save("robotDiscVars", "angTF_dDen","angTF_dNum","velTF_dDen","velTF_dNum");

figure();
step(velTF, '--', velTF_d, ':')
figure();
step(angTF, '--', angTF_d, ':')
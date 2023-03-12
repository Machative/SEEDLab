// Aidan Ferry and Charles Vath (I, Charles, based this document off of Aiden's code for the Miniproject and some prior code I wrote for step response testing in te Miniproject)
// 03/_/2023
// Demo 1 Identification Experiments

#include <Encoder.h>

#define EN1 4
#define M1DIR 7
#define M1SPD 9

#define EN2 5
#define M2DIR 8
#define M2SPD 10

// Logic variables
boolean ran = false;
boolean outBool = true;
boolean proc = false;
boolean turned=false;
double phiErrors[40] = {5, 5, 5};
double turnThresh = 0.01;

// Kp, Ki, Kd
double PhiP = 25; double PhiI = 0; double PhiD = 0.1;
double dPhiP = 1; double dPhiI = 0; double dPhiD = 0;
double DistP = 1; double DistI = 0; double DistD = 0.1;
double dDistP = 1; double dDistI = 0; double dDistD = 0;

// Error variables
double PhiE = 0; double PhidE = 0; double PhiintE = 0; double lastPhiE = 0;
double DistE = 0; double DistdE = 0; double DistintE = 0; double lastDistE = 0;
double dPhiE = 0; double dPhidE = 0; double dPhiintE = 0; double lastdPhiE = 0;
double dDistE = 0; double dDistdE = 0; double dDistintE = 0; double lastdDistE = 0;

// Voltage variables
double Vapp1=0; double Vapp2=0;

// Timing variables
// Subset of Timing: Global Experiment Clock
int clk;

// Subset of Timing: Error Timing
double presTime = 0; double prevTime = 0;

// Subset of Timing: Velocity Estimation
int newTime1 = 0  ; int newTime2 = 0;
int oldTime1; int oldTime2;

// Subset of Timing: March
int march; int thresh = 5000;
int marchStart; int marchEnd;

// Displacement variables
double setDist = 0; double setPhi = 0;
double disp1 = 0; double disp2 = 0;
double oldDisp1; double oldDisp2;
double presDist = 0; double presPhi = 0;

// Velocity variables
double setVel = 0; double setAngVel = 0;
double vel = 0; double rot = 0;
double om1; double om2;
double om1Arr[5] = {0}; double om2Arr[5] = {0};
double om1Avg = 0; double om2Avg = 0;
double setdPhi = 0; double setdDist = 0;

// Robot Paramters
double dist = 0.2794; // [m] 
double radii = 0.149/2; // [m]

// Other
double temp = 0;

//Initialize the motor encoder object
Encoder motorA(2,4);
Encoder motorB(3,5);

void setup() {
  Serial.begin(230400);
  
  //Initialize pins for motor driver control
  pinMode(EN1,OUTPUT);
  pinMode(M1DIR, OUTPUT);
  pinMode(M1SPD, OUTPUT);

  pinMode(EN2,OUTPUT);
  pinMode(M2DIR, OUTPUT);
  pinMode(M2SPD, OUTPUT);

  //Enable motor drivers
  digitalWrite(EN1,HIGH);
  digitalWrite(EN2,HIGH);

  // Clock for sampling
  clk = millis();

  // Timing
  oldTime1 = micros();
  oldTime2 = oldTime1;

  marchStart = oldTime1;
  marchEnd = oldTime1;

  prevTime = oldTime1;

  // Displacement
  oldDisp1 = 0;
  oldDisp2 = 0;  

}

void loop() {
  // Loop execution:
  // 1st, get clk and determine whether to apply a step response
  // 2nd, read the encoders and get the displacement, velocity, etc.. from the results
  // 3rd, PID control on inner and outer loop to dtermine voltage
  // 4th, convert PID values to voltage to apply to robot
  
  clk = millis();
  // Step response logic
  if (clk < 1000 && !ran) { // Set speed to 0
    setPhi = 0;
    setDist = 0;
  }
  
  else if (clk < 20000 && !ran) { // Apply step voltage as needed for avgVa or delVa
    setPhi = 1*PI; // Fudge factor () * angle in radians
    setDist = 1.03*3; // Fudge factor () * distance in meters
  }

  else { // Stop
    ran = true;
    setPhi = presPhi;
    setDist = presDist;
  }

  //Read Encoder 1
  disp1 = (double)motorA.read()*2*PI/3200;
  newTime1 = micros();
  om1 = 1000000*(disp1 - oldDisp1) / (newTime1 - oldTime1);

  //Read Encoder 2
  disp2 = -1*(double)motorB.read()*2*PI/3200;
  newTime2 = micros();
  om2 = 1000000*(disp2 - oldDisp2) / (newTime2 - oldTime2);

  // Handle clean-up from reading encoders
  oldTime1 = newTime1;
  oldDisp1 = disp1;
  
  oldTime2 = newTime2;
  oldDisp2 = disp2;
  
  // Process encoder data
  presDist = radii * (disp1 + disp2) / 2;
  presPhi = radii * (disp1 - disp2) / dist;

  // Moving average filter on angular velocity of motors to get more consistent results
  om1Avg = 0;
  om2Avg = 0;
  
  for(int i1 = 4; i1 > 0; i1 -= 1) {
    om1Avg += om1Arr[i1]/5.0;
    om1Arr[i1] = om1Arr[(i1-1)];
  }
  om1Avg += om1 / 4;
  om1Arr[0] = om1;

  for(int i1 = 4; i1 > 0; i1 -= 1) {
    om1Avg += om1Arr[i1]/5.0;
    om1Arr[i1] = om1Arr[(i1-1)];
  }
  om1Avg += om1 / 4;
  om1Arr[0] = om1;
  
  vel = radii * (om1 + om2) / 2;
  rot = radii * (om1 - om2) / dist;

  // PID Math for outer loop controller
  presTime = micros();
  PhiE = setPhi - presPhi;
  PhidE = (PhiE-lastPhiE)*1000000/(presTime-prevTime);
  PhiintE += PhiE*(presTime-prevTime)/1000000;
  lastPhiE = PhiE;
  
  DistE = setDist - presDist;
  DistdE = (DistE-lastDistE)*1000000/(presTime-prevTime);
  if ((abs(DistE) < 0.1) && (abs(PhiE) < 0.01)) {
    DistintE += DistE*(presTime-prevTime)/1000000;  
  }
  Serial.println(DistintE);
  lastDistE = DistE;

  setdPhi = PhiP*PhiE + PhiD*PhidE + PhiI*PhiintE;
  setdDist = DistP*DistE + DistD*DistdE + DistI*DistintE;

  // PID Math for inner loop controller
  dPhiE = setdPhi - rot;
  dPhidE = (dPhiE-lastdPhiE)*1000000/(presTime-prevTime);
  dPhiintE += dPhiE*(presTime-prevTime)/1000000;
  lastdPhiE = dPhiE;
  
  dDistE = setdDist - vel;
  dDistdE = (dDistE-lastdDistE)*1000000/(presTime-prevTime);
  dDistintE += dDistE*(presTime-prevTime)/1000000;
  lastdDistE = dDistE;  

  // Apply Voltage to turn angle first
  temp = 255.0 * (dPhiP*dPhiE + dPhiD*dPhidE + dPhiI*dPhiintE); 
  if (temp > 128) {temp = 128;}
  if (temp < -128) {temp = -128;}    
  
  Vapp1 = temp; 
  Vapp2 = -1*temp; 

  // Turning logic
  // Goal is for robot to turn only if past 40 phi errors were within a threshold range from 0.
  // Set turned to true, will be set false if not supposed to turn ideally

  // If the robot has turned already, give more slack for any bit of angular deviation
  turned = true;

  // Push all errors in array back 1 except for 0th error
  for(int i1=39; i1!=0; i1 -= 1) {
    if ((phiErrors[(i1-1)] > turnThresh) || (phiErrors[(i1-1)] < -1*turnThresh)) {
      turned = false;
    }
    phiErrors[i1] = phiErrors[(i1-1)];
  }

  // Add new error to 0th index and check if error exceeds thresh
  phiErrors[0] = PhiE;
  if ((phiErrors[0] > turnThresh) || (phiErrors[0] < -1*turnThresh)) {
      turned = false;
  }

  if (turned) {
    temp = 255.0 * (dDistP*dDistE + dDistD*dDistdE + dDistI*dDistintE); 
    if (temp > 128) {temp = 128;}
    if (temp < -128) {temp = -128;}    
    Vapp1 += temp; 
    Vapp2 += temp;
  }   

  // Saturation application to the voltage signal for both motors below
  if(Vapp1>255)Vapp1=255; 
  if(Vapp1<-255)Vapp1=-255; 

  if(Vapp2>255)Vapp2=255; 
  if(Vapp2<-255)Vapp2=-255;

  //Drive Motor
  if(Vapp1>0) digitalWrite(M1DIR,HIGH);
  else digitalWrite(M1DIR,LOW);

  if(Vapp2>0) digitalWrite(M2DIR,LOW);
  else digitalWrite(M2DIR,HIGH);
    
  analogWrite(M1SPD,abs(Vapp1));
  analogWrite(M2SPD,abs(Vapp2));
  
  // Print outs
  if (ran) {
    outBool = false;  
  }
  
  if (outBool) {
    Serial.print("|t:");
    Serial.print(clk);

    Serial.print("|V1:");
    Serial.print(Vapp1);

    Serial.print("|V2:");
    Serial.print(Vapp2);

    Serial.print("|PE:");
    Serial.print(PhiE);

    Serial.print("|DE:");
    Serial.print(DistE);

    //Serial.print("|dPE:");
    //Serial.print(dPhiE);

    //Serial.print("|dDE:");
    //Serial.print(dDistE);    

    //Serial.print("|d1:");
    //Serial.print(disp1);

    //Serial.print("|d2:");
    //Serial.print(disp2);

    Serial.println("");    
  } 

  // Sampling configuration
  // Get current time passed
  marchEnd = micros();
  march = marchEnd - marchStart;
  
  if (march > thresh) { // Went over sampling time
    Serial.println("DOST ERROR"); // DOST: Deviation Of Sampling Time
    Serial.println("");  
  }
  else {
    while (march < thresh) {
      marchEnd = micros();
      march = marchEnd - marchStart;  
    }  
  }

  marchStart = marchEnd;
}

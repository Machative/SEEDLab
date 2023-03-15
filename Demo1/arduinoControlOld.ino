//Aidan Ferry
//Date

#include <Wire.h>
#include <Encoder.h>

#define EN 4
#define M1DIR 7
#define M2DIR 8
#define M1SPD 9
#define M2SPD 10
#define PHI_THRESH 0.03
#define SLAVE_ADDRESS 0x04
#define CNTS_PER_REV 3200
#define MAX_SPEED 80

//Robot Parameters
float R = 7.5;
float b = 14; //b is the distance from wheel to axis of rotation, NOT distance between wheels

//Wheel Positions
double thetaL=0;
double thetaR=0;
//Current and Desired Position
double x=0;
double y=0;
double phi=0;
double desDist=20;
double desPhi=PI/2;
//Calculate desired X and Y coordinates based on desired distance and angle
double desX=desDist*cos(desPhi);
double desY=desDist*sin(desPhi);

Encoder motorLeft(2,4);
Encoder motorRight(3,5);

void setup() {
  //Begin Serial and Wire comms
  Serial.begin(230400);
  //Wire.begin(SLAVE_ADDRESS);

  //Initialize pins for motor driver control
  pinMode(EN,OUTPUT);
  pinMode(M1DIR, OUTPUT);
  pinMode(M2DIR, OUTPUT);
  pinMode(M1SPD, OUTPUT);
  pinMode(M2SPD, OUTPUT);

  digitalWrite(EN,HIGH); //Enable motor driver
}

void loop() {
  static uint32_t loopTime=0;
  loopTime=millis();
  static int16_t Vl=0;
  static int16_t Vr=0;
  static double wL=0;
  static double wR=0;
  static double v=0;
  static double w=0;
  static float laste=0;
  static uint32_t lastTime=0;
  static double thetaLlast=0;
  static double thetaRlast=0;
  static double dthetaL=0;
  static double dthetaR=0;
  static double ex=0;
  static double lastex=0;
  static double dex=0;
  static double intex=0;
  static double ey=0;
  static double intey=0;
  static double ephi=0;
  static double intephi=0;
  static uint8_t wait=0;

  //Read motor positions and convert to radians, and get changes from last readings
  thetaL = motorLeft.read()*2*PI/CNTS_PER_REV;
  dthetaL = thetaL - thetaLlast;
  thetaR = motorRight.read()*2*PI/CNTS_PER_REV;
  dthetaR = thetaR - thetaRlast;

  //Instantaneous Wheel Velocities (angular)
  wL = dthetaL*1000000/(micros()-lastTime);
  wR = dthetaR*1000000/(micros()-lastTime);

  //Errors
  ex = desX-x - wait*desX; //This wait variable is used to try to ignore the x and y errors until the turn is complete
  ey = desY-y - wait*desY;
  ephi = desPhi-phi;
  intex += ex*(double)(micros()-lastTime)/1000000;
  intey += ey*(double)(micros()-lastTime)/1000000;
  intephi += ephi*(double)(micros()-lastTime)/1000000;
  
  lastTime=micros();//Set last time as soon as you finish time-sensitive calculations

  //Update current position in (x,y,phi) coordinates based on differential drive kinematic equations
  x += (dthetaR+dthetaL)*(R/2)*cos(phi);
  y += (dthetaR+dthetaL)*(R/2)*sin(phi);
  phi = R*(thetaR-thetaL)/(2*b);
  
  v = R*(wR+wL)/2; //Translational velocity
  w = R*(wR-wL)/b; //Angular velocity

  static uint32_t angleCooldown=0;
  static double Kpv=1;
  static double Kpw=10;
  static double Kiv=0;
  static double Kiw=0;

  //If angle has settled for enough time, go ahead and pursue (x,y) position
  //If not, ignore x and y errors using wait command above
  if(ephi>PHI_THRESH) angleCooldown=micros();
  if(desPhi==0 || micros()-angleCooldown>2000000) wait=0;
  else wait=1;
  
  //If x and y errors are sufficiently low, stop integrating and just settle there
  if(abs(ex)<0.05 && abs(ey)<0.05) Kiv=0;
  else Kiv=1;
  
  //Control Structure
  //x and y error terms are projected on x and y axes based on current angle.
  //The +/- Kpw*ephi controls how velocity should be reduced according to a theta error (not increased because it will just saturate)
  Vl = (255.0/8)*(((Kpv*ex+Kiv*intex)*cos(phi)+(Kpv*ey+Kiv*intey)*sin(phi)) - Kpw*ephi - Kiw*intephi);
  Vr = (255.0/8)*(((Kpv*ex+Kiv*intex)*cos(phi)+(Kpv*ey+Kiv*intey)*sin(phi)) + Kpw*ephi + Kiw*intephi);
  
  //Saturate the speed at some speed limit
  if(Vl>MAX_SPEED) Vl=MAX_SPEED;
  if(Vl<-MAX_SPEED) Vl=-MAX_SPEED;
  if(Vr>MAX_SPEED) Vr=MAX_SPEED;
  if(Vr<-MAX_SPEED) Vr=-MAX_SPEED;

  //Print out current position
  Serial.print("x: ");
  Serial.print(x);
  Serial.print("\ty: ");
  Serial.print(y);
  Serial.print("\tphi: ");
  Serial.println(phi);

  //Drive Motor
  if(Vl>0) digitalWrite(M1DIR,LOW);
  else digitalWrite(M1DIR,HIGH);
  if(Vr>0) digitalWrite(M2DIR,HIGH);
  else digitalWrite(M2DIR,LOW);
  analogWrite(M1SPD,abs(Vl));
  analogWrite(M2SPD,abs(Vr));
  
  thetaLlast=thetaL;
  thetaRlast=thetaR;
  lastex=ex;

  if(millis()-loopTime<20) delay(20-(millis()-loopTime));
}

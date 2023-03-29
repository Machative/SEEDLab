//Aidan Ferry
//Date

#include <Wire.h>
#include <Encoder.h>

#define EN 4
#define M1DIR 7
#define M2DIR 8
#define M1SPD 9
#define M2SPD 10
#define SLAVE_ADDRESS 0x04
#define CNTS_PER_REV 3200
#define MAX_SPEED 100

#define idle 0
#define rotate 1
#define drive 2

//Robot Parameters
float R = 7.5;
float b = 14; //b is the distance from wheel to axis of rotation, NOT distance between wheels

Encoder motorLeft(2,4);
Encoder motorRight(3,5);

void setup() {
  //Begin Serial and Wire comms
  Serial.begin(9600);
  //Wire.begin(SLAVE_ADDRESS);

  //Initialize pins for motor driver control
  pinMode(EN,OUTPUT);
  pinMode(M1DIR, OUTPUT);
  pinMode(M2DIR, OUTPUT);
  pinMode(M1SPD, OUTPUT);
  pinMode(M2SPD, OUTPUT);

  digitalWrite(EN,HIGH); //Enable motor driver
  //Wire.onReceive(receiveData);
  //Wire.onRequest(sendData);
}

void loop() {
  static uint32_t loopTime=0;
  static double Vl=0;
  static double Vr=0;
  static double phi=0;
  static double dist=0;
  static double edist=0;
  static double ephi=0;
  static double thetaL=0;
  static double dthetaL=0;
  static double thetaR=0;
  static double dthetaR=0;
  static double thetaLlast=0;
  static double thetaRlast=0;
  static uint32_t lastTime=0;
  static uint8_t state=idle;
  loopTime=millis();
 
  thetaL = motorLeft.read()*2*PI/CNTS_PER_REV;
  dthetaL = thetaL - thetaLlast;
  thetaR = motorRight.read()*2*PI/CNTS_PER_REV;
  dthetaR = thetaR - thetaRlast;

  //Instantaneous Wheel Velocities (angular)
  //wL = dthetaL*1000000/(micros()-lastTime);
  //wR = dthetaR*1000000/(micros()-lastTime);

  lastTime=micros();//Set last time as soon as you finish time-sensitive calculations

  dist += (dthetaR+dthetaL)*(R/2);
  phi += R*(dthetaR-dthetaL)/(2*b); //Is this right?
  
  //v = R*(wR+wL)/2; //Instantaneous Translational velocity
  //w = R*(wR-wL)/b; //Instantaneous Angular velocity

  static double desDist=0;
  static double desPhi=0;

  //FSM
  if(state==idle){
    //Deal with receiving marker locations
    
    if(true){ //Begin rotating
      desDist=dist;
      desPhi=phi+(PI/2); //Desired rotation angle
      state=rotate;
    }
  }else if(state==rotate){
    //Deal with checking if rotation is "complete"

    if(false){//If done rotating and you want to drive
      desDist=dist;// + distToTarget (from camera)
      desPhi=phi;
      state=drive;
    }
  }else if(state==drive){
    //Deal with checking if drive is "complete"
    
    if(false){//If done driving and you want to return to idle
      desDist=dist;
      desPhi=phi;
      state=idle;
    }
  }

  //  //Errors
  edist = desDist-dist;
  ephi = desPhi-phi;
  //dex = ex*1000000/(micros()-lastTime);
  //intex += ex*(double)(micros()-lastTime)/1000000;

  static double Kpv=1;
  static double Kpw=1;

  Vl = (255.0/8)*(Kpv*edist - Kpw*ephi);
  Vr = (255.0/8)*(Kpv*edist + Kpw*ephi); //The +/- Kpw*ephi controls how velocity should be REDUCED according to a theta error (not increased because it will just saturate)

  if(Vl>MAX_SPEED) Vl=MAX_SPEED;
  if(Vl<-MAX_SPEED) Vl=-MAX_SPEED;
  if(Vr>MAX_SPEED) Vr=MAX_SPEED;
  if(Vr<-MAX_SPEED) Vr=-MAX_SPEED;

  //Drive Motor
  if(Vl>0) digitalWrite(M1DIR,LOW);
  else digitalWrite(M1DIR,HIGH);
  if(Vr>0) digitalWrite(M2DIR,HIGH);
  else digitalWrite(M2DIR,LOW);
  analogWrite(M1SPD,abs(Vl));
  analogWrite(M2SPD,abs(Vr));

  Serial.println(edist);
  
  thetaLlast=thetaL;
  thetaRlast=thetaR;

  if(millis()-loopTime<20) delay(20-(millis()-loopTime));
}

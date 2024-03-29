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

//States to be used for Finite State Machine
#define search 0
#define halt 1
#define rotate 2
#define drive 3
#define idle 4

//Robot Parameters
float R = 7.5;
float b = 14; //b is the distance from wheel to axis of rotation, NOT distance between wheels

bool newVisionPhi=false;
uint32_t lastPixelRead=0;
uint32_t haltTime=0;
uint32_t offTarget=0;
uint32_t waitTime=0;
double visionPhi=0;
uint16_t visionPixels=0;

uint8_t state=search; //Begin program by searching for marker

Encoder motorLeft(2,4);
Encoder motorRight(3,5);

void setup() {
  //Begin Serial and Wire comms
  Serial.begin(9600);
  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(recieveData);
  Wire.onRequest(sendData);

  //Initialize pins for motor driver control
  pinMode(EN,OUTPUT);
  pinMode(M1DIR, OUTPUT);
  pinMode(M2DIR, OUTPUT);
  pinMode(M1SPD, OUTPUT);
  pinMode(M2SPD, OUTPUT);

  digitalWrite(EN,HIGH); //Enable motor driver
}

void sendData(){
  if(state==drive || state==rotate) Wire.write(1); //If state is drive or rotate, receive marker height
  else Wire.write(0); //If searching or halting, receive marker angle
}

void recieveData(int byteCount) {
  if(state==halt || state==search || state==idle){ //Interpret I2C data as marker angle based on agreed-upon protocol
    int8_t bytes[byteCount];
    uint8_t i=0;
    while(Wire.available()){
      bytes[i]=Wire.read();
      i++;
    }
    visionPhi = (bytes[1] + bytes[2]/10.0)*(PI/180.0);
    newVisionPhi = true;
  }
  else { //Interpret I2C data as pixel height
    int8_t bytes[byteCount];
    uint8_t i=0;
    while(Wire.available()){
      bytes[i]=Wire.read();
      i++;
    }
    uint8_t pixelsHigh = bytes[1];
    uint8_t pixelsLow = bytes[2];
    visionPixels = pixelsHigh*256 + pixelsLow;
    lastPixelRead=millis();
  }
}

void loop() {
  static uint32_t loopTime=0;
  static double Vl=0;
  static double Vr=0;
  static double phi=0;
  static double dist=0;
  static double edist=0;
  static double ephi=0;
  static double dphi=0;
  static double intedist=0;
  static double intephi=0;
  static double thetaL=0;
  static double dthetaL=0;
  static double thetaR=0;
  static double dthetaR=0;
  static double thetaLlast=0;
  static double thetaRlast=0;
  static uint32_t lastTime=0;
  loopTime=millis();

  //Get current angle and change in angle of each wheel
  thetaL = motorLeft.read()*2*PI/CNTS_PER_REV;
  dthetaL = thetaL - thetaLlast;
  thetaR = motorRight.read()*2*PI/CNTS_PER_REV;
  dthetaR = thetaR - thetaRlast;

  //Adjust distance and angle of robot accordingly
  dist += (dthetaR+dthetaL)*(R/2);
  phi += R*(dthetaR-dthetaL)/(2*b); //Is this right?

  static double desDist=0;
  static double desPhi=0;

  //Calculate errors for PI control
  edist = desDist-dist;
  ephi = desPhi-phi;
  dphi = ephi*1000000/(micros()-lastTime);
  if(edist<1) intedist += edist*(double)(micros()-lastTime)/1000000;
  if(ephi<0.1) intephi += ephi*(double)(micros()-lastTime)/1000000;

  lastTime=micros();//Set last time as soon as you finish time-sensitive calculations

  //Control parameters (some of these are unused as they are specifically tuned to each state)
  static double Kpv=8;
  static double Kiv=2;
  static double Kpw=8;
  static double Kiw=13;

  //FSM
  if(state==search){
    //Deal with receiving marker locations
    if(newVisionPhi && Vl==0 && Vr==0) state=halt; //If marker is visible immediately, go to halt which will then go to rotate
    Vl = (255.0/8)*(-8*ephi - 6*intephi);
    Vr = (255.0/8)*(8*ephi + 6*intephi);
    if(abs(ephi)>0.05){
      offTarget=millis();
    }
    if(millis()-offTarget>200){ //If error is settled for 200ms, go to halt and check for marker
      state=halt;
      haltTime=millis();
    }
    waitTime=millis();
  }else if(state==halt){
    Vl=0;
    Vr=0;
    if(!newVisionPhi && millis()-haltTime>1000) { //If halted for 1s and no marker, go back to search
      state=search;
      desPhi = phi + 35.0*PI/180.0;
      intephi=0;
    }
    if(newVisionPhi && millis()-waitTime>2000){ //If marker found, allow camera to catch up. After 2s, get angle and go to rotate state
      desDist=dist;
      desPhi=phi+visionPhi; //Desired rotation angle
      intephi=0;
      state=rotate;
    }
  }else if(state==rotate){
    Vl = (255.0/8)*(-6*ephi - 10*intephi);
    Vr = (255.0/8)*(6*ephi + 10*intephi);
    if(abs(ephi) >= 0.04){
      offTarget=millis();
    }
    if(millis()-offTarget>1000){ //If error is settled for 1s, go to drive
      desDist=dist-((2511.7/pow(visionPixels,0.987))-10); //Calculate distance from marker height in pixels according to math model
      desPhi=phi;
      state=drive;
    }
  }else if(state==drive){
    Vl = (255.0/8)*(Kpv*edist + Kiv*intedist - 5*ephi);
    Vr = (255.0/8)*(Kpv*edist + Kiv*intedist + 5*ephi);
    if(abs(edist)<0.5) state=idle; //If done driving, go to idle
  }else if(state==idle){
    Vl=0;
    Vr=0;
  }

  sendData();

  //Speed limit depends on which state the robot is in
  static uint8_t speedLimit=0;
  if(state==drive) speedLimit=85;
  else speedLimit=70; 
  if(Vl>speedLimit) Vl=speedLimit;
  if(Vl<-speedLimit) Vl=-speedLimit;
  if(Vr>speedLimit) Vr=speedLimit;
  if(Vr<-speedLimit) Vr=-speedLimit;
  
  //Drive Motor
  if(Vl>0) digitalWrite(M1DIR,LOW);
  else digitalWrite(M1DIR,HIGH);
  if(Vr>0) digitalWrite(M2DIR,HIGH);
  else digitalWrite(M2DIR,LOW);
  analogWrite(M1SPD,abs(Vl));
  analogWrite(M2SPD,abs(Vr));

  thetaLlast=thetaL;
  thetaRlast=thetaR;

  if(millis()-loopTime<20) delay(20-(millis()-loopTime));
}

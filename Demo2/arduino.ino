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
#define MAX_SPEED 80
#define SEARCH_SPEED 30
#define APPROACH_SPEED 40

#define search 0
#define halt 1
#define rotate 2
#define drive 3
#define idle 4

//Robot Parameters
float R = 7.5;
float b = 14; //b is the distance from wheel to axis of rotation, NOT distance between wheels

bool newVisionPhi=false;
uint16_t lastPixelRead=0;
uint16_t haltTime=0;
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
  //Wire.onRequest(sendData);
}

void sendData(){
  if(state==drive) Wire.write(1);
  else Wire.write(0);
}

void recieveData(int byteCount) {
  if(state==rotate || state==search || state==idle){
    int8_t bytes[byteCount];
    uint8_t i=0;
    while(Wire.available()){
      bytes[i]=Wire.read();
      i++;
    }
    visionPhi = (bytes[1] + bytes[2]/10.0)*(PI/180.0);
    newVisionPhi=true;
//    Serial.print(visionPhi*(180.0/PI));
//    Serial.println(" degrees");
  }
  else if(state==drive){
    int8_t bytes[byteCount];
    uint8_t i=0;
    while(Wire.available()){
      bytes[i]=Wire.read();
      i++;
    }
    uint8_t pixelsHigh = bytes[1];
    uint8_t pixelsLow = bytes[2];
    visionPixels = pixelsHigh*256 + pixelsLow;
//    Serial.print(visionPixels);
//    Serial.println(" pixels");
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

  //Errors
  edist = desDist-dist;
  ephi = desPhi-phi;
  //dex = ex*1000000/(micros()-lastTime);
  intedist += edist*(double)(micros()-lastTime)/1000000;
  intephi += ephi*(double)(micros()-lastTime)/1000000;

  static double Kpv=1;
  static double Kiv=2;
  static double Kpw=3;
  static double Kiw=15;

  //FSM
  if(state==search){
    //Deal with receiving marker locations
    if(newVisionPhi) state=halt; //If marker is visible immediately, go to halt which will then go to rotate
    Vl = (255.0/8)*(-Kpw*ephi - Kiw*intephi);
    Vr = (255.0/8)*(Kpw*ephi + Kiw*intephi);
    if(abs(ephi)<0.03){
      state=halt;
      haltTime=millis();
    }
  else if(state==halt){
    Vl=0;
    Vr=0;
    if(millis()-haltTime>1000) {
      state=search;
      desPhi = phi + 30.0*PI/180.0;
    }
    if(newVisionPhi){ // If marker found
      newVisionPhi=false;
      desDist=dist;
      desPhi=phi+visionPhi; //Desired rotation angle
      state=rotate;
    }
  }
  }else if(state==rotate){
    //Deal with checking if rotation is "complete"
    Vl = (255.0/8)*(-Kpw*ephi - Kiw*intephi);
    Vr = (255.0/8)*(Kpw*ephi + Kiw*intephi);
    if(abs(ephi) < 0.03){//If done rotating and you want to drive
      desDist=dist;// + distToTarget (from camera)
      desPhi=phi;
      state=drive;
    }
  }else if(state==drive){
    //Deal with checking if drive is "complete"
    //Vl = (255.0/8)*(Kpv*edist + Kiv*intedist - Kpw*ephi - Kiw*intephi);
    //Vr = (255.0/8)*(Kpv*edist + Kiv*intedist + Kpw*ephi + Kiw*intephi);
    Vl=-APPROACH_SPEED;
    Vr=-APPROACH_SPEED;
    if(visionPixels>125 || millis()-lastPixelRead > 1000){//If done driving
      desDist=dist;
      desPhi=phi;
      state=idle;
    }
  }else if(state==idle){
    Vl=0;
    Vr=0;
  }

  sendData();
  
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

  thetaLlast=thetaL;
  thetaRlast=thetaR;

  if(millis()-loopTime<20) delay(20-(millis()-loopTime));
}

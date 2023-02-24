#include <Encoder.h>

#define EN 4
#define M1DIR 7
#define M1SPD 9

double Kp=0.28;
double Kd=0;
double Ki=0.016;

double pos=0;
double desiredPos=0;
double vel = 4*PI/3200;

int clk;
int march; int marchStart; int marchEnd; int thresh = 5000; 
int newT; int oldT; int delT;
int spd = 0;
double period = 0.005;
double oldDisp; double rads;
bool ran = false;
double acce = 0;

Encoder motorA(2,4);

void setup() {
  Serial.begin(230400);
  
  pinMode(EN,OUTPUT);
  pinMode(M1DIR, OUTPUT);
  pinMode(M1SPD, OUTPUT);

  digitalWrite(EN,HIGH);
  digitalWrite(M1DIR,LOW);//LOW = CLKWISE

  // Initialization
  // Timing
  clk = millis();
  newT = micros();
  oldT = micros();
  marchStart = micros();
  marchEnd = micros();

  march = marchEnd - marchStart;

  // Displacement
  oldDisp = 0;  
}

void loop() {
  static int16_t Vapp=0;
  static float laste=0;
  static uint32_t lastTime=0;
  static float e = 0;
  static float de = 0;
  static float inte = 0;

  //Read Encoder
  pos = (double)motorA.read()*2*PI/3200;

  // Convert to velocityc
  rads = 1000000*(pos - oldDisp) / (micros() - lastTime);
  oldDisp = pos;

  //PID Math
  e = desiredPos - pos;
  de = (e-laste)*1000000/(micros()-lastTime);
  inte += + e*(micros()-lastTime)/1000000;
  Vapp = 255.0 * (Kp*e + Kd*de + Ki*inte);
  if(Vapp>255)Vapp=255; //Saturation
  if(Vapp<-255)Vapp=-255;

  //Drive Motor
  if(Vapp>0) digitalWrite(M1DIR,LOW);
  else digitalWrite(M1DIR,HIGH);
  analogWrite(M1SPD,abs(Vapp));

  laste=e;
  lastTime=micros();

  newT = micros();
  delT = newT - oldT;
  // Activate ramp function for position only at specified times
  if (clk < 1000 && !ran) { // Set speed to 0
    desiredPos = 0;
  }
  
  else if (clk < 4000 && !ran) { // Apply step of 1 V
    desiredPos = desiredPos + vel*delT/1000;
    spd = 1;
  }

  else { // Stop
    desiredPos = desiredPos;
    spd = 0;
    ran = true;
  }
  oldT = newT;
  
  // Print outs
  Serial.print("i:");
  Serial.print(inte);  
  
  Serial.print("|t:");
  Serial.print(clk);
  
  Serial.print("|B:");
  Serial.print(spd);

  Serial.print("|w:");
  Serial.print(rads);

  Serial.print("|d:");
  Serial.print(desiredPos);  

  Serial.print("|x:");
  Serial.print(pos);  

  Serial.println("");

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

  // Get new clock
  clk = millis();
}

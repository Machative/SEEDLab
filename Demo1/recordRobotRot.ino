#include <Encoder.h>

#define EN1 4
#define M1DIR 7
#define M1SPD 9

#define EN2 5
#define M2DIR 8
#define M2SPD 10

// Logic variables
boolean outBool = true;

// Timing variables
int newTime1 = 0; int newTime2 = 0;
int oldTime1; int oldTime2;

// Subset of Timing: March
int march; int thresh = 5000;
int marchStart; int marchEnd;

// Displacement variables
double disp1 = 0; double disp2 = 0;
double oldDisp1; double oldDisp2;

double dist = 0.275; // [m] 
double radii = 0.149/2; // [m]

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

  // Displacement
  oldDisp1 = 0;
  oldDisp2 = 0;  

}

void loop() {
  
  //Read Encoder 1
  disp1 = (double)motorA.read()*2*PI/3200;
  newTime1 = micros();

  //Read Encoder 2
  disp2 = -1*(double)motorB.read()*2*PI/3200;
  newTime2 = micros();

  // Handle clean-up from reading encoders
  oldTime1 = newTime1;
  oldDisp1 = disp1;
  
  oldTime2 = newTime2;
  oldDisp2 = disp2;
  
  if (true) {

    Serial.print("|d1:");
    Serial.print(disp1);

    Serial.print("|d2:");
    Serial.print(disp2);

    Serial.print("|m1");
    Serial.print(motorA.read());

    Serial.print("|m2");
    Serial.print(motorB.read());

    Serial.println("");
  } 
}

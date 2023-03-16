// Aidan Ferry and Charles Vath (I, Charles, based this document off of Aiden's code for the Miniproject and some prior code I wrote for step response testing in te Miniproject)
// 03/_/2023
// Demo 1 Identification Experiments

// Encoder library
#include <Encoder.h>

// Define motor pins
#define EN1 4
#define M1DIR 7
#define M1SPD 9

#define EN2 5
#define M2DIR 8
#define M2SPD 10

// Logic variables
boolean ran = false;
boolean outBool = true;

// Voltage variables
// avgVa and delVa decide which variable is being measured. One should be 8 and the other 0. Vapp is for the voltage fed into the motors.
double avgVa = 0; double delVa = 8;
double Vapp1=0; double Vapp2=0;

// Timing variables
int clk; // Used to implement step response and stop
int newTime1 = 0  ; int newTime2 = 0;
int oldTime1; int oldTime2;

// Subset of Timing: March. Used to keep time at ~5 ms for the loop
int march; int thresh = 5000;
int marchStart; int marchEnd;

// Displacement variables record motor wheel displacement
double disp1 = 0; double disp2 = 0;
double oldDisp1; double oldDisp2;

// Velocity variables
double vel = 0; double rot = 0;
double om1; double om2;

// Robot physical parameters
double dist = 0.275; // [m] 
double radii = 0.149/2; // [m]

//Initialize the motor encoder object
Encoder motorA(2,4);
Encoder motorB(3,5);

void setup() {
  // Serial communication
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
  oldTime2 = micros();

  marchStart = micros();
  marchEnd = micros();

  // Displacement
  oldDisp1 = 0;
  oldDisp2 = 0;  

}

void loop() {
  clk = millis();
  // Step response logic
  if (clk < 1000 && !ran) { // Set speed to 0
    Vapp1 = 0;
    Vapp2 = 0;
  }
  
  else if (clk < 4000 && !ran) { // Apply step voltage as needed for avgVa or delVa
    // Average Va = 1, ergo Vapp1 and Vapp2 = 0.5 V each. Delta Va = 1, ergo Vapp1 = 0.5 V and Vapp2 = -0.5 V.
    Vapp1 = (255.0/8) * (avgVa + delVa)/2;
    Vapp2 = (255.0/8) * (avgVa - delVa)/2;
  }

  else { // Stop
    Vapp1 = 0;
    Vapp2 = 0;
    ran = true;
  }

  //Drive Motor
  if(Vapp1>0) digitalWrite(M1DIR,HIGH);
  else digitalWrite(M1DIR,LOW);

  if(Vapp2>0) digitalWrite(M2DIR,LOW);
  else digitalWrite(M2DIR,HIGH);
    
  analogWrite(M1SPD,abs(Vapp1));
  analogWrite(M2SPD,abs(Vapp2));

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
  vel = radii * (om1 + om2) / 2;
  rot = radii * (om1 - om2) / dist;
  
  // Print outs and print logic
  if (clk > 8000 && ran) {
    outBool = false;  
  }
  
  if (outBool) {
    Serial.print("|t:");
    Serial.print(clk);

    Serial.print("|v:");
    Serial.print(vel);

    Serial.print("|w:");
    Serial.print(rot);

    Serial.print("|d1:");
    Serial.print(disp1);

    Serial.print("|d2:");
    Serial.print(disp2);

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
  else { // wait until 5 ms
    while (march < thresh) {
      marchEnd = micros();
      march = marchEnd - marchStart;  
    }  
  }

  marchStart = marchEnd;
}

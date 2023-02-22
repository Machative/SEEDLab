#include <Encoder.h>

#define EN 4
#define M1DIR 7
#define M1SPD 9

Encoder motorA(2,4);

int clk;
int march; int thresh = 5000; int newT; int oldT;
int spd = 0;
double period = 0.005;
double oldDisp; double rads;

void setup() {
  Serial.begin(230400);
  
  pinMode(EN,OUTPUT);
  pinMode(M1DIR, OUTPUT);
  pinMode(M1SPD, OUTPUT);

  digitalWrite(EN,HIGH);
  digitalWrite(M1DIR,LOW);//LOW = CLKWISE
  analogWrite(M1SPD,spd);

  // Initialization
  // Timing
  clk = millis();
  newT = micros();
  oldT = micros();

  march = newT - oldT;

  // Displacement
  oldDisp = 0;
}

void loop() {
  // Activate motor only at specified times
  if (clk < 1000) { // Set speed to 0
    spd = 0;
  }
  
  else if (clk < 2000) { // FULL SPEED AHEAD
    spd = 255;
  }

  else { // Stop
    spd = 0;
  }

  analogWrite(M1SPD,spd);  
  
  //Read Encoder
  double newDisp = (double)motorA.read()*2*PI/3200;

  // Convert to velocity
  rads = (newDisp - oldDisp) / (period);
  oldDisp = newDisp;

  // Print outs
  Serial.print("Time [ms]: ");
  Serial.println(clk);
  
  Serial.print("Motor Logic: ");
  Serial.println(spd);

  Serial.print("Angular Velocity [rad/s]: ");
  Serial.println(rads);

  //Serial.print("Accumulated Disp.");
  //Serial.println(newDisp);  

  Serial.println("");

  // Sampling configuration
  // Get current time passed
  newT = micros();
  march = newT - oldT;
  
  if (march > thresh) { // Went over sampling time
    Serial.println("DOST ERROR"); // DOST: Deviation Of Sampling Time
    Serial.println("");  
  }
  else {
    while (march < thresh) {
      newT = micros();
      march = newT - oldT;  
    }  
  }

  oldT = newT;

  // Get new clock
  clk = millis();
}

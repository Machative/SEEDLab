//Aidan Ferry
//24 February 2023
//Mini Project Localization Contribution
//This code controls a motor connected to the Arduino through a motor shield. It also receives a value between 1 and 4 over I2C communication.
//These values each indicate a position to which the motor should rotated (1 = 0 radians, 2 = PI/2 radians, 3 = PI radians, 4 = 3PI/2 radians).
//The Arduino also sends back the current precise position of the motor over I2C.
//The motor is driven with a generic PID controller, however in this application the I and D terms were not needed and so were set to zero.
//This is because the system is essentially a first-order system with little mass and lots of natural damping.

#include <Encoder.h>
#include <Wire.h>

#define EN 4
#define M1DIR 7
#define M1SPD 9
#define SLAVE_ADDRESS 0x04

//PID Control Constants
double Kp=3;
double Kd=0;
double Ki=0;

//Current and Desired Position
double pos=0;
double desiredPos=0;

//Initialize the motor encoder object
Encoder motorA(2,4);

void setup() {
  //Begin Serial and Wire comms
  Serial.begin(9600);
  Wire.begin(SLAVE_ADDRESS);

  //Initialize pins for motor driver control
  pinMode(EN,OUTPUT);
  pinMode(M1DIR, OUTPUT);
  pinMode(M1SPD, OUTPUT);

  digitalWrite(EN,HIGH); //Enable motor driver
  Wire.onReceive(receiveData);
  Wire.onRequest(sendData);
}

//This function is called when the Arduino receives a message over I2C.
//The function parses the message and interprets it as a desired motor position.
void receiveData(int byteCount){
  while(Wire.available()) {
    uint8_t quadrant = Wire.read();
    switch(quadrant){
      case 1:
        desiredPos=0;
        break;
      case 2:
        desiredPos=PI/2;
        break;
      case 3:
        desiredPos=PI;
        break;
      case 4:
        desiredPos=3*PI/2;
        break;
    }
  }
}

//This function is responsible for sending data over I2C.
//It parses the current motor position and sends each of the three digits as bytes in an array.
void sendData(){
  byte posArr[3];
  posArr[0] = (uint8_t)pos;
  posArr[1] = ((uint8_t)(pos*10))%10;
  posArr[2] = ((uint8_t)(pos*100))%10;
  Wire.write(posArr, 3);
}

void loop() {
  static int16_t Vapp=0;
  static float laste=0;
  static uint32_t lastTime=0;
  static float e=0;
  static float de=0;
  static float inte=0;

  //Read Encoder
  pos = (double)motorA.read()*2*PI/3200;

  //Calculate the P, I, and D terms for PID control
  e = pos-desiredPos; //Radians
  de = (e-laste)*1000000/(micros()-lastTime); //Radians per second
  inte += ((e+laste)/2)*(micros()-lastTime)/1000000; //Radians times seconds

  //Calculate the applied voltage (in the form of a duty cycle) based on the PID controller
  Vapp = 255.0/8 * (Kp*e + Kd*de + Ki*inte); //Volts
  if(Vapp>255)Vapp=255; //Saturate the duty cycle at +/- 255 if it is above/below these values.
  if(Vapp<-255)Vapp=-255;

  //Drive Motor
  if(Vapp>0) digitalWrite(M1DIR,HIGH);
  else digitalWrite(M1DIR,LOW);
  analogWrite(M1SPD,abs(Vapp));

  laste=e;
  lastTime=micros();
  
  sendData();

  delay(20);
}

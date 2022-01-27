//
// Include the Arduino Stepper Library
#include <Stepper.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>


// Number of steps per output rotation
const static int stepsPerRevolution = 200;

const static int Temp1 = 2;
const static int Temp2 = 3;
const static int Temp3 = 5;

const static int relay_1 = 4;
const static int relay_2 = 7;
const static int switch_ir = 8;
const static int switch_cam = 12;

const static int stepper_speed = 60; // The speed is set in RPM 
const static int serial_speed = 9600; 

bool cam_stat = false;
bool IR_stat = true;

double h = 70.0;

Stepper myStepper(stepsPerRevolution, 6, 9, 10, 11);
OneWire t_w1(Temp1);
DallasTemperature t_s1(&t_w1);

int deviceCount = 0;
float tempC;

void setup() {
 
  myStepper.setSpeed(stepper_speed);
  Serial.begin(serial_speed);
  Serial.println("**********************************************");
  Serial.println("* Welcome to the ARDUINO Observatory control *");
  Serial.println("*                MPC-CODE: M15               *");
  Serial.println("**********************************************");
  
  pinMode(switch_cam, OUTPUT);
  digitalWrite(switch_cam, LOW);
  
  pinMode(switch_ir, OUTPUT);
  digitalWrite(switch_ir, LOW);
  t_s1.begin();
  
  delay(500);
  t_s1.setResolution(12);
  
  deviceCount = t_s1.getDeviceCount();
  Serial.print("Temperature sensors: ");
  Serial.println(deviceCount); 

  ir_lamp(IR_stat);

}


void loop() {

  String in = Serial.readStringUntil("\n");

  if((in == String("Camera\n")) || (in == String("camera\n"))) {
    qhypower();
  }
  if ((in == String("Focuser\n")) || (in == String("focuser\n"))) {
    turn_focuser();
  }

  if( h > 80. ){
    ir_lamp(true);  
  }
  if( h < 50. ) {
    ir_lamp(false);
  }

  get_temperatures();

  
}


void get_temperatures() {
  t_s1.requestTemperatures();

  for(int i=0; i<deviceCount; i++){
    Serial.print("Sensor ");
    Serial.print(i+1);
    Serial.print(" : ");
    Serial.print(t_s1.getTempCByIndex(i));
    Serial.println("C");
  }
  Serial.println(" ");
  Serial.println(" ");
}


void ir_lamp(bool IR) {

  if(IR) {
    digitalWrite(switch_ir, HIGH);
  } else {
    digitalWrite(switch_ir, LOW);
  }
}




void qhypower() {
  
  Serial.println("Camera Setting");
  Serial.print("Camera power status: ");
  Serial.println(cam_stat);
  Serial.println("What do you want to do? (use 0/1 to switch off/on, respectively");
  while(Serial.available() == 0) {}
  String cam_status_now = Serial.readStringUntil("\n");
  int rel_stat = cam_status_now.toInt();

  if (rel_stat == 1) cam_stat == true;
  else cam_stat = false;
  
  digitalWrite(switch_cam, rel_stat);

  Serial.print("Camera power status: ");
  Serial.println(cam_stat);
  
}


void turn_focuser() {
  Serial.println("Focusing, insert the number of rounds: ");
  while(Serial.available() == 0) {
  }
  String rounds_c = Serial.readStringUntil("\n");
  int rounds = rounds_c.toInt();
  Serial.print("Move the focuser of ");
  Serial.print(rounds);
  Serial.print(" rounds .... ");
  
  Serial.print(" steps...");
  myStepper.step(rounds);
  Serial.println(" DONE");
  digitalWrite(6, LOW);
  digitalWrite(9, LOW);
  digitalWrite(10, LOW);
  digitalWrite(11, LOW);
}

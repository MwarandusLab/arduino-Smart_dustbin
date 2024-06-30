//Mwarandus Lab

#include <SoftwareSerial.h>

//stepper Motor Connection
const int STEPPER_PIN_1 = 10;
const int STEPPER_PIN_2 = 11;
const int STEPPER_PIN_3 = 12;
const int STEPPER_PIN_4 = 13;

const int STEPS_PER_REVOLUTION = 2048;
const float DEGREES_PER_STEP = 360.0 / STEPS_PER_REVOLUTION;
const int TARGET_DEGREES = 40;

//IR Sensor Variables
int IR_1 = 8;
int IR_2 = 9;

int SensorValue_1 ;
int SensorValue_2 ;

//Moisture Sensor Variables
int Moisture_1 = A4;
int Moisture_2 = A5;

int MoistureValue_1 ;
int MoistureValue_2 ;

//RX pin & TX pin of GSM Module (SIM800L)
int TX = 2;
int RX = 3;

int Sent = 0; // Checks if text is already sent or not to avoid sending multiple texts 

//ultrasonic sensor 1
const int trigPin_1 = 5; // Trigger pin of the ultrasonic sensor
const int echoPin_1 = 4; // Echo pin of the ultrasonic sensor

long duration_1 ;
long distance_1 ;

//ultrasonic sensor 2
const int trigPin_2 = 7; // Trigger pin of the ultrasonic sensor
const int echoPin_2 = 6; // Echo pin of the ultrasonic sensor

long duration_2 ;
long distance_2 ;

//Create software serial object to communicate with SIM800L
SoftwareSerial mySerial(TX, RX); //SIM800L Tx & Rx is connected to Arduino #3 & #2


void setup() {
  Serial.begin(9600); // Initialize the serial communication
  mySerial.begin(9600); //Begin serial communication with Arduino and SIM800L

  pinMode(trigPin_1, OUTPUT); // Set the trigger pin_1 as output
  pinMode(echoPin_1, INPUT); // Set the echo pin_1 as input

  pinMode(trigPin_2, OUTPUT); // Set the trigger pin_2 as output
  pinMode(echoPin_2, INPUT); // Set the echo pin_2 as input

  pinMode(IR_1, INPUT); //sets IR sensor 1 as Output
  pinMode(IR_2, INPUT); //Sets IR sensor 2 as Output

  pinMode(STEPPER_PIN_1, OUTPUT);
  pinMode(STEPPER_PIN_2, OUTPUT);
  pinMode(STEPPER_PIN_3, OUTPUT);
  pinMode(STEPPER_PIN_4, OUTPUT);

  mySerial.println("AT"); //Once the handshake test is successful, it will back to OK
  updateSerial();

}

void loop() {
  //Read IR sensor Values
  SensorValue_1 = digitalRead(IR_1);
  SensorValue_2 = digitalRead(IR_2);

  //Ultrasonic Sensor 1 Code
  digitalWrite(trigPin_1, LOW); // Set the trigger pin to low
  delayMicroseconds(2);
  digitalWrite(trigPin_1, HIGH); // Set the trigger pin to high
  delayMicroseconds(10);
  digitalWrite(trigPin_1, LOW); // Set the trigger pin to low
  duration_1 = pulseIn(echoPin_1, HIGH); // Measure the pulse duration of the echo pin
  distance_1 = duration_1 * 0.034 / 2; // Calculate the distance in centimeters
  Serial.print("Distance_1: "); // Print the distance to the serial monitor
  Serial.print(distance_1);
  Serial.println(" cm");
  delay(500); // Wait for 500 milliseconds before taking the next measurement

  //Ultrasonic Sensor 2 Code
  digitalWrite(trigPin_2, LOW); // Set the trigger pin to low
  delayMicroseconds(2);
  digitalWrite(trigPin_2, HIGH); // Set the trigger pin to high
  delayMicroseconds(10);
  digitalWrite(trigPin_2, LOW); // Set the trigger pin to low
  duration_2 = pulseIn(echoPin_2, HIGH); // Measure the pulse duration of the echo pin
  distance_2 = duration_2 * 0.034 / 2; // Calculate the distance in centimeters
  Serial.print("Distance_2: "); // Print the distance to the serial monitor
  Serial.print(distance_2);
  Serial.println(" cm");
  delay(500); // Wait for 500 milliseconds before taking the next measurement

  //Checks Distance from ultrasonic sensor
  if(distance_1 < 8 && Sent == 0){ //Dry Section
    mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
    updateSerial();
    mySerial.println("AT+CMGS=\"+254748613509\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
    updateSerial();
    mySerial.print("SMART BIN (DRY): HALF FULL"); //text content
    updateSerial();
    mySerial.write(26);
    Sent = 1;
    delay(2000);
  }else if(distance_2 < 8 && Sent == 0){ //Wet Section
    mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
    updateSerial();
    mySerial.println("AT+CMGS=\"+254748613509\"");//change ZZ with country code and xxxxxxxxxxx with phone number to sms
    updateSerial();
    mySerial.print("SMART BIN (WET): HALF FULL"); //text content
    updateSerial();
    mySerial.write(26);
    Sent = 1;
    delay(2000);
  }else{
    //Do nothing
    Sent = 0;
  }

  //Checks weather the wet or dry litter is detected and instruct the motor to rotate
  if(digitalRead(IR_1) == LOW || digitalRead(IR_2) == LOW ){
     //Read Soil Moisture Sensor Values
    MoistureValue_1 = analogRead(Moisture_1);
    MoistureValue_2 = analogRead(Moisture_2);
    delay(1000);
    if(MoistureValue_1 < 900 || MoistureValue_2 < 900){
      delay(1000);
      rotateAntiClockwise();
      delay(1000);
    }else if(MoistureValue_1 > 1005 || MoistureValue_2 > 1005){
      delay(1000);
      rotateClockwise();
      delay(1000);
    }else{
      //Do nothing
    }
  }


}

void updateSerial()
{
  delay(500);
  while (Serial.available()) 
  {
    mySerial.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while(mySerial.available()) 
  {
    Serial.write(mySerial.read());//Forward what Software Serial received to Serial Port
  }
}
//Motor Function To rotate clockwise
void rotateClockwise() {
  int targetSteps = TARGET_DEGREES / DEGREES_PER_STEP;
  const int stepSequence[8][4] = {
    {HIGH, LOW, LOW, LOW},
    {HIGH, HIGH, LOW, LOW},
    {LOW, HIGH, LOW, LOW},
    {LOW, HIGH, HIGH, LOW},
    {LOW, LOW, HIGH, LOW},
    {LOW, LOW, HIGH, HIGH},
    {LOW, LOW, LOW, HIGH},
    {HIGH, LOW, LOW, HIGH}
  };
  for (int i = 0; i < targetSteps; i++) {
    for (int j = 0; j < 8; j++) {
      digitalWrite(STEPPER_PIN_1, stepSequence[j][0]);
      digitalWrite(STEPPER_PIN_2, stepSequence[j][1]);
      digitalWrite(STEPPER_PIN_3, stepSequence[j][2]);
      digitalWrite(STEPPER_PIN_4, stepSequence[j][3]);
      delayMicroseconds(1000);
    }
  }
  delay(500);
  for (int i = 0; i < targetSteps; i++) {
    for (int j = 7; j >= 0; j--) {
      digitalWrite(STEPPER_PIN_1, stepSequence[j][0]);
      digitalWrite(STEPPER_PIN_2, stepSequence[j][1]);
      digitalWrite(STEPPER_PIN_3, stepSequence[j][2]);
      digitalWrite(STEPPER_PIN_4, stepSequence[j][3]);
      delayMicroseconds(1000);
    }
  }
  digitalWrite(STEPPER_PIN_1, LOW);
  digitalWrite(STEPPER_PIN_2, LOW);
  digitalWrite(STEPPER_PIN_3, LOW);
  digitalWrite(STEPPER_PIN_4, LOW);
}
//Motor Function to ratate anticlockwise
void rotateAntiClockwise() {
  int targetSteps = TARGET_DEGREES / DEGREES_PER_STEP;
  const int stepSequence[8][4] = {
    {HIGH, LOW, LOW, HIGH},
    {LOW, LOW, LOW, HIGH},
    {LOW, LOW, HIGH, HIGH},
    {LOW, LOW, HIGH, LOW},
    {LOW, HIGH, HIGH, LOW},
    {LOW, HIGH, LOW, LOW},
    {HIGH, HIGH, LOW, LOW},
    {HIGH, LOW, LOW, LOW}  
    
  };
  for (int i = 0; i < targetSteps; i++) {
    for (int j = 0; j < 8; j++) {
      digitalWrite(STEPPER_PIN_4, stepSequence[j][3]);
      digitalWrite(STEPPER_PIN_3, stepSequence[j][2]);
      digitalWrite(STEPPER_PIN_2, stepSequence[j][1]);
      digitalWrite(STEPPER_PIN_1, stepSequence[j][0]);
      delayMicroseconds(1000);
    }
  }
  delay(500);
  for (int i = 0; i < targetSteps; i++) {
    for (int j = 7; j >= 0; j--) {
      digitalWrite(STEPPER_PIN_4, stepSequence[j][3]);
      digitalWrite(STEPPER_PIN_3, stepSequence[j][2]);
      digitalWrite(STEPPER_PIN_2, stepSequence[j][1]);
      digitalWrite(STEPPER_PIN_1, stepSequence[j][0]);
      delayMicroseconds(1000);
    }
  }
  digitalWrite(STEPPER_PIN_4, LOW);
  digitalWrite(STEPPER_PIN_3, LOW);
  digitalWrite(STEPPER_PIN_2, LOW);
  digitalWrite(STEPPER_PIN_1, LOW);
}


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Adafruit_NeoPixel.h>
#include <Servo.h>

/*
*     CONFIGURE WIRELESS SSID AND PASS
*/
char ssid[] = "ssid";
char pass[] = "pass";

//Authorization for corresponding Blynk mobile application
const char auth[] = "86897338237640b18c0fd5effa6e156c";

#define BUTTON    V0
#define JOYSTICK  V1

const int m1=5;
const int r1=4;
const int l1=0;
const int l2=14;
const int r2=12;
const int m2=16;

const int trig=15;
const int echo=13;
const float min_dist=20;

byte buttonMode=1; //buttonMode 1 signifies self-driving state
char carState='s';

Servo servo; //creates servo object

void setup() {
  Serial.begin(9600);
  Serial.print("setup");
  Blynk.begin(auth, ssid, pass);
  pinMode(m1, OUTPUT);
  pinMode(m2, OUTPUT);
  pinMode(l1, OUTPUT);
  pinMode(l2, OUTPUT);
  pinMode(r1, OUTPUT);
  pinMode(r2, OUTPUT);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  servo.attach(2);
  servo.write(90);
  
}

void loop() {
  Blynk.run();
  Serial.print("start");
  if(buttonMode==1){
    selfDrive();
  }
  else if(carState=='s'){
    stopCar();
  }
  else if(carState=='f'){
    moveForward();
  }
  else if(carState=='b'){
    moveBack();
  }
  else if(carState=='l'){
    moveLeft();
  }
  else if(carState=='r'){
    moveRight();
  }
  
}

void moveBack(){
  Serial.println("Backward");
  digitalWrite(l1, LOW);
  digitalWrite(r1, HIGH);
  analogWrite(m1, 1280);
  digitalWrite(l2, LOW);
  digitalWrite(r2, HIGH);
  analogWrite(m2, 1280);
}

void moveForward(){
  Serial.println("Forward");
  digitalWrite(l1, HIGH);
  digitalWrite(r1, LOW);
  analogWrite(m1, 1280);
  digitalWrite(l2, HIGH);
  digitalWrite(r2, LOW);
  analogWrite(m2, 1280);
}

void moveLeft(){
  Serial.println("Right");
  digitalWrite(l1, LOW);
  digitalWrite(r1, LOW);
  analogWrite(m1, 1280);
  digitalWrite(l2, HIGH);
  digitalWrite(r2, LOW);
  analogWrite(m2, 1280);
}

void moveRight(){
  Serial.println("Left");
  digitalWrite(l1, HIGH);
  digitalWrite(r1, LOW);
  analogWrite(m1, 1280);
  digitalWrite(l2, LOW);
  digitalWrite(r2, LOW);
  analogWrite(m2, 1280);
}

void stopCar(){
  Serial.println("Stopped");
  digitalWrite(l1, LOW);
  digitalWrite(r1, LOW);
  analogWrite(m1, 1280);
  digitalWrite(l2, LOW);
  digitalWrite(r2, LOW);
  analogWrite(m2, 1280);
}

void selfDrive(){
  Serial.println("Self-Driving");
  if(readDistance() < min_dist) {
    stopCar();
    turn();
  }
  moveForward();
}

BLYNK_WRITE(BUTTON){
    int i=param.asInt();
    if (i==1){
      Serial.println("mode is 0");
      buttonMode=0;
    }
    else{
      Serial.println("mode is 1");
      buttonMode=1;
    }
}
BLYNK_WRITE(JOYSTICK) {
  int y=param[0].asInt();
  int x=param[1].asInt();
  
  if(buttonMode==0){
    if(x<200){
      carState='b';
    }
    else if(x>800){
      carState='f';
    }
    else if(y<200){
      carState='l';
    }
    else if(y>800){
      carState='r';
    }
    else{
      carState='s';
    }
  }
}

void turn(){
  //check left
  servo.write(0);
  delay(200);
  float dist = readDistance();
  Serial.print(dist);
  if(dist > min_dist){
    servo.write(90);
    delay(200);
    moveRight();
    delay(750);
    stopCar();
    if(dist > min_dist) return;
  }
  servo.write(180);
  delay(200);
  dist = readDistance();
  Serial.print(dist);
  if(dist > min_dist){
    servo.write(90);
    delay(100);
    moveLeft();
    delay(750);
    stopCar();
    if(dist > min_dist) return;
  }
  // cannot move left or right
  // alternative: turn around?
  servo.write(90);
  delay(200);
  moveBack();
  delay(750);
  stopCar();
}

//Method from DistanceSensor lab
float readDistance() { 
  digitalWrite(trig, LOW); delayMicroseconds(2);
  digitalWrite(trig, HIGH); delayMicroseconds(10);
  digitalWrite(trig, LOW);
  unsigned long timeout=micros()+26233L;
  while((digitalRead(echo)==LOW)&&(micros()<timeout));
  unsigned long start_time = micros();
  timeout=start_time+26233L;
  while((digitalRead(echo)==HIGH)&&(micros()<timeout));
  unsigned long lapse = micros() - start_time;
  return lapse*0.01716f;
}



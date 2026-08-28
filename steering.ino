#include<Servo.h>
#include<LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);
int motor1pin1 = 13;
int motor1pin2 = 7;
int motor1speed = 9; //PWM
int motor2pin1 = 8;
int motor2pin2 = 10;
int motor2speed = 6; //PWM
int servopin = 5;
Servo servo;
int potsteer = A2;
int potspeed = A1;
int trigR = 11; 
int echoR = 12;
int trigL = 4;
int echoL = 3;
int buttonpin = 2; //must be pin 2 or 3
float distanceR;
float distanceL;
volatile bool direction = true;
int printingbutton = A3;
int buttonvalue = 0;
String status ;
void setup() {

lcd.init();
lcd.clear();
lcd.backlight();
servo.attach(servopin);
pinMode(motor1pin1,OUTPUT);
pinMode(motor1pin2,OUTPUT);
pinMode(motor1speed,OUTPUT);
pinMode(motor2pin1,OUTPUT);
pinMode(motor2pin2,OUTPUT);
pinMode(motor2speed,OUTPUT);
pinMode(trigR,OUTPUT);
pinMode(trigL,OUTPUT);
pinMode(echoR,INPUT);
pinMode(echoL,INPUT);
pinMode(buttonpin,INPUT_PULLUP);
attachInterrupt(digitalPinToInterrupt(buttonpin),ISR_changedirection,FALLING);
Serial.begin(9600);
}

void loop() {

//steering potentiometer
  int potValue = analogRead(potsteer);
  int angle = map(potValue,0,1023,0,180);
  servo.write(angle);

//speed potentiometer
  int pot2Value = analogRead(potspeed);
  int PWM = map(pot2Value,0,1023,0,255);
  int RPM = map(pot2Value,0,1023,0,600); // will be displayed

//ultrasonic reading and failsafe
  distanceR=ultra(trigR,echoR);
  distanceL=ultra(trigL,echoL);
  bool nearbydanger =(distanceR<=20 || distanceL<=20);
  if (nearbydanger){
  stop();
  status = "stopped";
} else {
  if(direction){
  moveforward(PWM);
  status = "forward";
} else {
  movebackward(PWM);
  status = "backward";
}
}

// vehicle speed calculation by m/s
float wheelradius =0.0345;
float vehiclespeed= 2 * 3.14159 * wheelradius * (RPM / 60.0); // will be displayed
//making RPM and speed equals zero if the car is stopped
if(status=="stopped"){
  RPM =0;
  vehiclespeed=0;
}
// displaying motor status & speed & rpm
buttonvalue=analogRead(printingbutton);
if(buttonvalue<512){
Serial.println(status);
Serial.println(vehiclespeed);
Serial.println(RPM);

// lcd printinggg
lcd.setCursor(0,0);
lcd.print(status);
lcd.setCursor(0,1);
lcd.print("speed = ");
lcd.print(vehiclespeed);
lcd.print("m/s");
delay(3000);
lcd.clear();
lcd.setCursor(0,0);
lcd.print("rpm = ");
lcd.print(RPM);
delay(3000);
lcd.clear();
}
}

void moveforward(int PWM){
  digitalWrite(motor1pin1,HIGH);
  digitalWrite(motor1pin2,LOW);
  digitalWrite(motor2pin1,HIGH);
  digitalWrite(motor2pin2,LOW);
  for(int i=0;i<PWM;i+=5){
  analogWrite(motor1speed,i);
  analogWrite(motor2speed,i);
  delay(5);
  }
}

void movebackward(int PWM){
   digitalWrite(motor1pin1,LOW);
   digitalWrite(motor1pin2,HIGH);
   digitalWrite(motor2pin1,LOW);
   digitalWrite(motor2pin2,HIGH);
for(int i=0;i<PWM;i+=5){
  analogWrite(motor1speed,i);
  analogWrite(motor2speed,i);
  delay(5);
  }
}
void stop(){
  digitalWrite(motor1pin1,LOW);
  digitalWrite(motor1pin2,LOW);
  digitalWrite(motor2pin1,LOW);
  digitalWrite(motor2pin2,LOW);
}
float ultra(int trig , int echo){
  digitalWrite(trig,LOW);
  delayMicroseconds(2);
  digitalWrite(trig,HIGH);
  delayMicroseconds(10);
  digitalWrite(trig,LOW);
long duration= pulseIn(echo,HIGH);
float distance= (duration * 0.0343)/2;
return distance;
}
// button changing direction
void ISR_changedirection(){
static long lastTime=0;
long currentTime=millis();
if(currentTime - lastTime > 200){
  direction=!direction;
}
lastTime = currentTime;
}

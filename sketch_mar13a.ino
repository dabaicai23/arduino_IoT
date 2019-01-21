#include "DHT.h" //온습도
#include <MsTimer2.h>
#include <DS1302.h>//리얼타임
#include <SoftwareSerial.h>//시리얼통신 라이브러리 호출
#include <Servo.h>
SoftwareSerial mySerial = SoftwareSerial(2,3);
DHT dht(4,DHT11);//온도습도센서
//Servo myservo;

int ledPin = 13;//
int light1 = 5;//렐리 1
int light2 = 6;//렐리 2
int human = 9;//인체감지
int rain = 0;//물방울
int sensorPin = A3;//심장박동
int sensorP = 0;
int ave = 0;
int dustPin = A2;//미세먼지
int brightness = A0;

int Win = 0;
int Light = 0;
int a = 0;//불 자동 감지
int b = 0;
int lightime = 0;
int winoff = 0;
int Wins = 0;

float  dustVal = 0;
float dustDensity = 0;

int ledPower  =12;
int delayTime  = 280;
int delayTime2 = 240;
float offTime = 9680;

//심박
float heart = 0;


float t, h;//온도 습도선언

String myString = "";
boolean autolight = 0;
byte Tord[2] = {};
byte ord[2] = {};
int count = 0;
boolean flag = true;

int sp1=11;//servo motor pin
int pulsewidth;//
int val;
int val1;
int myangle1;

void servopulse(int sp1,int val1)//
{
  myangle1=map(val1,0,180,500,2480);
  digitalWrite(sp1,HIGH);//
  delayMicroseconds(myangle1);//
  digitalWrite(sp1,LOW);//
  delay(20-val1/1000);
}

void AutoLightAndWin(){
  rain = analogRead(A1);
  a = digitalRead(9);
  b = analogRead(A0);
   if(Light == 1 && a == 1&& b < 200){
    lightime = 1000;
      digitalWrite(light1,LOW);
      digitalWrite(light2,LOW);
      //Serial.println("Light  on");
    }
    if(Win == 1 && rain < 500){
      if(Wins == 0&&winoff == 0){
          winoff = 1;
          Win = 1;
      }else{
        winoff = 0;
      }
      //Serial.println("Windows  off");
    }
    if( Light == 1 &&lightime == 0){
      digitalWrite(light1,HIGH);
      digitalWrite(light2,HIGH); 
    }   
}
int Pulse(){//심박즉정

  int rawValue = 0;
  int arr = 0;
  for(int i = 0;i < 10;i++){
    rawValue = analogRead(sensorPin);
    //ave = map(rawValue,0,1024,0,200);
    Serial.println(rawValue);
    //value = alpha * oldValue + (1 - alpha) * rawValue;//这个平滑就是取本次和上一次测量数据的加权平均值
    arr += rawValue;
    delay(20); 
  }
  heart = arr / 10;

}
void printTH(){//온도습도즉정
     t = dht.readTemperature();  
     h = dht.readHumidity();   
}

float air(){ 
  digitalWrite(ledPower,LOW);
  delayMicroseconds(delayTime);
  dustVal=analogRead(dustPin);
  delayMicroseconds(delayTime2);
  digitalWrite(ledPower,HIGH);
  delayMicroseconds(offTime);
  delay(1000);
  
  Serial.println(dustVal);
  dustDensity = 0.17*(dustVal*0.0049)-0.1;
  Serial.print("Dust density(mg/m3) = ");
  Serial.println(dustDensity);
 
  return dustDensity;
  delay(1000);
}
void setup() {
  // put your setup code here, to run once:
  dht.begin();
  Serial.begin(9600);
  mySerial.begin(9600);
  while(!mySerial);
  mySerial.write(0xFF);
  digitalWrite(ledPin,HIGH);
  pinMode(human,INPUT);//인체감지
  pinMode(brightness,INPUT);//조도센서
  pinMode(light1,OUTPUT);//불
  pinMode(light2,OUTPUT);
  pinMode(sp1,OUTPUT);  
  //myservo.attach(11);
  //myservo.write(0);

  digitalWrite(light1,HIGH);
  digitalWrite(light2,HIGH);

  MsTimer2::set(2000,AutoLightAndWin);//3초마다 인체감지체크하고 LED제어
  MsTimer2::start();//시작
  
}

void loop() {
  // put your main code here, to run repeatedly:
    if(lightime > 0){
      lightime  = lightime - 1;
    }
    if(winoff == 1){
      for(int i = 0;i <50;i++){
          servopulse(sp1,0);//
        }
        Wins  = 1;
    }
    delay(10);
    if(mySerial.available()){  //블루투스에서 넘어온 데이터가 있다면
        ord[count++] = mySerial.read();
        if(count == 2);
        flag = false;       
      }
      if(flag)
        return ;
      flag = true;
      count = 0;

    //Serial.println("Bluetooth: ");
    //Serial.println(ord[0]);
    //Serial.println(ord[1]);
    
   switch(ord[0]){

    case 0:
      
        break;
    case 1:
        printTH();
        Tord[0] = 1;
        Tord[1] = t;
        mySerial.write(Tord,2);
        break;
    case 2:
        printTH();
        Tord[0] = 2;
        Tord[1] = h;
        mySerial.write(Tord,2);
        break;       
    case 3://open light1
        autolight == 0;
        digitalWrite(light1,LOW);
        Tord[0] = 3;
        Tord[1] = 0;
        mySerial.write(Tord,2);
        break;
    case 4://open light2
        autolight == 0;
        digitalWrite(light2,LOW);
        Tord[0] = 4;
        Tord[1] = 0;
        mySerial.write(Tord,2);
        break;
    case 5://off light1
        autolight == 0;
        digitalWrite(light1,HIGH);
        Tord[0] = 5;
        Tord[1] = 0;
        mySerial.write(Tord,2);
        break;
    case 6://off light2
        autolight == 0;
        digitalWrite(light2,HIGH);
        Tord[0] = 6;
        Tord[1] = 0;
        mySerial.write(Tord,2); 
        break; 
    case 7://불 자동 상태 켜짐
        Light = 1;
        Tord[0] = 7;
        Tord[1] = 0;
        mySerial.write(Tord,2);
        break;
    case 8://air quality index
         air();
        Tord[0] = 8;
        Tord[1] = air() * 100;
        mySerial.write(Tord,2);
        break; 
    case 9://불 자동 상태 꺼짐
        Light = 0;
        Tord[0] = 9;
        Tord[1] = 0;
        mySerial.write(Tord,2);       
        break;
    case 10://불  다 꺼짐
        digitalWrite(light1,HIGH);
        digitalWrite(light2,HIGH);
        Tord[0] = 10;
        Tord[1] = 0;
        mySerial.write(Tord,2);   
        break;
    case 11:
        delay(2500);
        Pulse();
        Tord[0] = 11;
        //ave = map(sensorP,0,1024,0,600);
        Tord[1] = heart;
        mySerial.write(Tord,2);
        break;
    case 12://win open
        Tord[0] = 12;
        Tord[1] = 0;
        mySerial.write(Tord,2);
        delay(4500);   
        for(int i = 0;i <50;i++){
          servopulse(sp1,180);//
        }
        Wins = 0;          
        break;
    case 13://win off
        Tord[0] = 13;
        Tord[1] = 0;
        mySerial.write(Tord,2);
        delay(4500);
        for(int i = 0;i <50;i++){
          servopulse(sp1,0);//
        }
        //myservo.write(0);   
        break;
    case 14://win auto off
        Win = 0;
        Tord[0] = 14;
        Tord[1] = 0;
        mySerial.write(Tord,2);
        break;
    case 15:// win auto on
          Win = 1;
        Tord[0] = 15;
        Tord[1] = 0;
        mySerial.write(Tord,2);
        break;
    default:
    
          return;
   }

}


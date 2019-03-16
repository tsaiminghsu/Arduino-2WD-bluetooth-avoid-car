#include <Ultrasonic.h>
#include <Servo.h>
// 這是一套可以通用市面上 Android APP 的程式碼
// 藍牙 HC-05(6) 密碼：1234
// Google Play 商店 app 下載連結
// https://play.google.com/store/apps/details?id=braulio.calle.bluetoothRCcontroller&hl=en

//********************定義 define ********************
//   定義藍牙移動上的可能性
//   定義伺服馬達的角度
//   定義移動時可能會用到的參數
//   在參數裡面添加類似代碼減少程式碼和複雜性
//   通過區域變數來減少使用全域變數
//********************Bluetooth接法********************
/*
Arduino        Bluetooth        備註
5V             Vcc              注意電源不可接錯
GND            GND              注意電源不可接錯
Rx(Pin 0)      Tx
Tx(Pin 1)      Rx
****************************************
適用於Arduino Uno
//超聲波接線：TRIGD3，ECHOD2
//超音波小車（ARDUINO）
//    L = 左　R = 右　F = 前　B = 後
****************************************
*/
#define IN1   5
#define IN2   6
#define IN3   10
#define IN4   11
#define SERVO_PIN   9 
//***********************定義馬達及紅外線腳位*************************
const int MotorRight1   = IN1;
const int MotorRight2   = IN2; 
const int MotorLeft1    = IN3; 
const int MotorLeft2    = IN4;
const int servoPin = SERVO_PIN; 
//*************************定義超音波腳位******************************
int inputPin = 2;  // 定義超音波信號接收腳位 RX
int outputPin = 3;  // 定義超音波信號發射腳位 TX
int Fspeedd = 0;      // 前速
int Rspeedd = 0;      // 右速
int Lspeedd = 0;      // 左速
int directionn = 0;   // 前=8 後=2 左=4 右=6 
Servo myservo; // 定義伺服馬達為 myservo
const int delay_time   = 250;  //伺服馬達轉向後的穩定時間
const int Fgo = 8; // 前進
const int Rgo = 6; // 右轉
const int Lgo = 4; // 左轉
const int Bgo = 2; // 倒車
//*************************定義PWM馬達速度*************************
int iSpeed = 180;
//*************************定義藍牙讀取信號*************************
char command; 
//*************************定義HC-SR04超音波測距離*************************
float cmMsec, inMsec;

Ultrasonic ultrasonic(inputPin, outputPin);
  
void setup() {
  Serial.begin(9600);   // 9600是與電腦間進行溝通，請勿更改
  pinMode(IN1, OUTPUT);  
  pinMode(IN2, OUTPUT);  
  pinMode(IN3,  OUTPUT);  
  pinMode(IN4,  OUTPUT);  

  myservo.write(90);  // 定義伺服馬達輸出腳位(PWM)
  myservo.attach(servoPin);
}

//**************Movement functions******************************
void advance(int d)  //前進
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  delay(d * 100);
}
void back(int d)  //後退
{ 
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  delay(d * 100);
}
void right(int d)   //右轉(單輪)
{ 
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  delay(d * 100);
}
void left(int d)  //左轉(單輪)  
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  delay(d * 100);
}
void turnR(int d)  //右轉(雙輪)
{
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  delay(d * 100);
}
void turnL(int d)  //左轉(雙輪)
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  delay(d * 100);
}
void stopp(int d)  //停止
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  delay(d * 100);
}

void test(){
  long microsec = ultrasonic.timing();
  cmMsec = ultrasonic.convert(microsec, Ultrasonic::CM); // 計算距離，單位: 公分
  inMsec = ultrasonic.convert(microsec, Ultrasonic::IN); // 計算距離，單位: 英吋
  Serial.print("MS: ");
  Serial.print(microsec);
  Serial.print(", CM: ");
  Serial.print(cmMsec);
  Serial.print(", IN: ");
  Serial.println(inMsec);
  delay(500);
}
void ask_pin_F()   // 量出前方距離 
    {
      myservo.write(90);
      test();              // 將距離 讀入Fspeedd(前速)
      Fspeedd = cmMsec;
    }  
void ask_pin_L()   // 量出左邊距離 
    {
      myservo.write(177);
      delay(delay_time);
      test();              // 將距離 讀入Lspeedd(左速)
      Lspeedd = cmMsec;
    }  
void ask_pin_R()   // 量出右邊距離 
    {
      myservo.write(5);
      delay(delay_time);
      test();              // 將距離 讀入Rspeedd(右速)
      Rspeedd = cmMsec;
    }      
void detection() {        //測量3個角度(0.90.179)     
    int delay_time = 250;   // 伺服馬達轉向後的穩定時間
    ask_pin_F();            // 讀取前方距離
    if(Fspeedd < 15)         // 假如前方距離小於10公分
    {
      stopp(1);               // 清除輸出資料 
      back(2);                // 後退 0.2秒
    }      
    if(Fspeedd < 25)         // 假如前方距離小於25公分
    {
      stopp(1);               // 清除輸出資料 
      ask_pin_L();            // 讀取左方距離
      delay(delay_time);      // 等待伺服馬達穩定
      ask_pin_R();            // 讀取右方距離  
      delay(delay_time);      // 等待伺服馬達穩定        
      if(Lspeedd > Rspeedd)   //假如 左邊距離大於右邊距離
      {
         directionn = Rgo;      //向右走
      }        
      if(Lspeedd <= Rspeedd)   //假如 左邊距離小於或等於右邊距離
      {
         directionn = Lgo;      //向左走
      }    
      if (Lspeedd < 15 && Rspeedd < 15)   //假如 左邊距離和右邊距離皆小於10公分
      {
         directionn = Bgo;      //向後走        
      }          
    }
    else                      //加如前方不小於(大於)25公分     
    {
       directionn = Fgo;        //向前走     
    }
}
void BTcommand(){
  command = (char) Serial.read();
  Serial.println(command);
  Serial.write(command);
}
void autoRun() {
  myservo.write(90); // make the servo motor reset
  int value = 1;//設定要不要繼續超音波避障 value = 1 的時候迴圈繼續; value = 0 的時候迴圈停止
  while(value){
    BTcommand();
    if (command == 'S' || command == 's' || command == 'v') { //停止不使用
       value = 0;
    }
    detection();
    if (directionn == Fgo) 
    { //go straight
      advance(1); 
      Serial.print(" ->Advance "); 
    }
    else if (directionn == Bgo) 
    { //go back
      back(8); 
      turnL(3); 
      Serial.print(" ->Reverse "); 
    }
    else if (directionn == Rgo)
    { //turn right
      back(1);
      turnR(6); 
      Serial.print(" ->Right "); 
    }
    else if (directionn == Lgo) 
    { //turn left
      back(1);
      turnL(6);
      Serial.print(" ->Left ");            
    }
  }
}
//*************************Android 手機 APP 藍牙遙控*************************
void loop() {
  myservo.write(90);  //讓伺服馬達回歸 預備位置 準備下一次的測量
  //advance(0);
  if (Serial.available()) {
    if (Serial.available()) { //檢查藍牙指令讀取
      BTcommand();
      if (command == 'F') { // 前進
        advance(0);
      }
      else if (command == 'B') { // 後退
        back(0);
      }
      else if (command == 'R') { // 往右
        right(0);
      }
      else if (command == 'L') { // 往左
        left(0);
      }
      else if (command == 'S' || command == 's' || command == 'v') { //停止不使用
        stopp(0); 
      }
      else if (command == 'V')
      {
        autoRun();
        Serial.println("Auto");
      }
      else if (int(command) >= 49 && int(command) <= 57) 
      { 
        iSpeed = (int(command)-48)*26;        //自動設定馬達速度(PWM) - 請注意這裡只用在
        Serial.println("Speed set to: " + iSpeed); 
      }
      else
      {
        stopp(0);
      }
    }
    else
    {
      stopp(0);
    }
  }
}

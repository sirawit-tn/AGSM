// library
#include "Nextion.h"
#include <esp_task_wdt.h>
#include <BH1750FVI.h>
#include <ESP32Servo.h>
//#include <esp_task_wdt.h>
#include <SoftwareSerial.h>
#include <TridentTD_LineNotify.h>
#include <IOXhop_FirebaseESP32.h>

//connec firebase
#define FIREBASE_HOST "https://garbage-machine-e3da8-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "s0qREsx09x30DFUTywVXEC7Ei3LVoK1VropTRQJU"
#define LINE_TOKEN "7jOZlqNu1GYWtA2aw2OWrlcKIl4DVs4lVZa1aDzQE4x"
#define WIFI_SSID "Mi 10T"
#define WIFI_PASSWORD "a88498fcc64a"

//Nextion
NexButton bPage0 = NexButton(0, 1, "bPage0");//ปุ่มหน้าแรก
NexButton bst = NexButton(1, 2, "bst"); //page, id , objname *start*
NexText tid = NexText(1, 1, "tid");// รหัสน.ศ.
NexText t0 = NexText(1, 3, "t0");// สถานะล็อคอิน
NexText tsid = NexText(2, 3, "tsid"); // show student_id
NexText show_ = NexText(5, 3, "show");
NexText tgwifi = NexText(14, 1, "tgwifi"); //show รหัสเข้าสุ่ระบบ wifi
NexText t1 = NexText(13, 2, "t1"); // เครือข่าย
NexText t2 = NexText(13, 3, "t2"); // รหัสไวไฟ
NexPage GoTOPage0 = NexPage(0, 0, "0");//หน้าแรก
NexPage GoTOPage1 = NexPage(1, 0, "1");//login
NexPage GoTOPage6 = NexPage(6, 0, "6");//wait
NexPage GoTOPage2 = NexPage(2, 0, "2");//Show Score
NexPage GoTOPage3 = NexPage(3, 0, "3");//รอทิ้งขยะ
NexPage GoTOPagecan = NexPage(7, 0, "7");//can
NexPage GoTOPageplastic = NexPage(8, 0, "8");//plastic
NexPage GoTOPageother = NexPage(9, 0, "9");//other
NexPage GoTOPage9 = NexPage(10, 0, "10");// FULL
NexPage GoTOPage4 = NexPage(4, 0, "4");// ตัดสินใจทิ้งต่อไหม
NexPage GoTOPage10 = NexPage(11, 0, "11");//เน็ตหลุด
NexPage GoTOPage11 = NexPage(12, 0, "12"); //ป็อปอัพwifi
NexPage GoTOPage12 = NexPage(13, 0, "13");// wifi connect
NexPage GoTOPage13 = NexPage(14, 0, "14"); //login wifi
NexNumber nCan = NexNumber(2, 7, "nCan");
NexNumber nPlastic = NexNumber(2, 8, "nPlastic");
NexNumber nOther = NexNumber(2, 9, "nOther");
NexButton bb = NexButton(2, 1, "bb"); // back จากหน้าคะแนน
NexButton bn = NexButton(2, 2, "bn"); // ปุ่มทิ้งขยะ
NexButton byes = NexButton(4, 1, "byes"); // ปุ่มทิ้งขยะอีกครั้ง
NexButton bwifi = NexButton(1, 5, "bwifi"); //ปุ่มไวไฟ หน้าเข้าสู่ระบบ
NexButton bnou = NexButton(1, 4, "bnou"); //ทิ้งแบบไม่เป็นสมาชิก
NexButton bnowifi = NexButton(11, 1, "bnowifi"); //ปุ่มไวไฟ หน้าหลุด
NexButton bcwifi = NexButton(13, 4, "bcwifi"); // ปุ่ม connect wifi
NexButton bon = NexButton(14, 2, "bon"); //ปุ่มเข้าระบบไวไฟ
char buffer[100] = {0};
char LoginWIFI[20], SSIDWIFI[30], PASSWIFI[30];
NexTouch *nex_listen_list[] =
{
  &bst,
  &bPage0,
  &bb,
  &bn,
  &byes,
  &bwifi,
  &bnou,
  &bnowifi,
  &bcwifi,
  &bon,
  NULL
};

//light sensor
BH1750FVI LightSensor(BH1750FVI::k_DevModeContLowRes);
Servo servo_1;
Servo servo_2;

//scaner
SoftwareSerial mySerial(32, 35); // TX, RX Barcode Scanner
#define PINOpenScanner 15
String Student_Card_Str = "";
char Student_Card_Chr[30];
//Cooldown page 0
bool StartScan = false, CooldownP0 = true; //opop = false;
unsigned long backMillis = 0, dimMillis = 0;
//Line
bool line_check = false;
static uint8_t cm1, cm2, cm3;

// limit sw
#define sw1 26
#define sw2 25
#define sw3 27
#define in1 12
#define in2 14
int sw1State,
    sw2State,
    sw3State;
//Ultrasonic
#define echoPIN1 18
#define trigPIN1 19
#define echoPIN2 0
#define trigPIN2 2
#define echoPIN3 5
#define trigPIN3 4

//กระบวนการคัดแยก
int can, plastic, other;
bool chekst = false, check = false;
String Cycle = "wait"; //plastic/can/other/wait
bool userLogin, ProcessAGM = false,connecWIFI = false;

TaskHandle_t firebase_;

void setup() {
  nexInit();
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  GoTOPage10.show();
  while (WiFi.status() != WL_CONNECTED) {
    static int i = 0;
    delay(500);
    Serial.print(".");
    if (i == 10) {
      i = 0;
      WiFi.reconnect();
    }
    i++;
  }
  GoTOPage0.show();
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(echoPIN1, OUTPUT);
  pinMode(trigPIN1, INPUT);
  pinMode(echoPIN2, OUTPUT);
  pinMode(trigPIN2, INPUT);
  pinMode(echoPIN3, OUTPUT);
  pinMode(trigPIN3, INPUT);
  pinMode(sw1, INPUT);
  pinMode(sw2, INPUT);
  pinMode(sw3, INPUT);
  GoTOPage0.show();
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  LINE.setToken(LINE_TOKEN);
  servo_1.attach(33);
  servo_2.attach(23);
  mySerial.begin(9600);
  LightSensor.begin();
  bPage0.attachPush(bPage0PopCallback, &bPage0);
  bst.attachPush(bstPopCallback, &bst);
  bb.attachPush(bbPopCallback, &bb);
  bn.attachPush(bnPopCallback, &bn);
  byes.attachPush(byesPopCallback, &byes);

  pinMode(PINOpenScanner, OUTPUT);
  digitalWrite(PINOpenScanner, HIGH);
  digitalWrite(in1, HIGH);
  servo_1.write(240);
  servo_2.write(18);
  while (true) {
    Limitswitch();
    Serial.println(sw3State);
    if (sw1State == 1) {
      MotorLeft();
    } else if (sw2State == 1) {
      MotorRight();
    } else if (sw3State == 1) {
      MotorStop();
      break ;
    } delay(1);
  }
  xTaskCreatePinnedToCore(loop_two, "firebase_", 20000, NULL, 1, &firebase_, 1);//แยกคอร์ในการทำงาน
}
void MotorConTrol(String txt) {
  if (txt == "L") {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
  } else if (txt == "R") {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
  } else {
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
  }
}
void servo_down() {
  servo_2.write(100);
  delay(1000);
  servo_2.write(18);
}

// ฟังก์ชันอัลตร้าโซนิค
int Ultrasonic(byte echoPIN, byte trigPIN) {  //ฟังก์ชั่นอ่านค่าระยะทาง
  long duration, distance;
  digitalWrite(echoPIN, LOW);
  delayMicroseconds(2);
  digitalWrite(echoPIN, HIGH);
  delayMicroseconds(5);
  digitalWrite(echoPIN, LOW);
  duration = pulseIn(trigPIN, HIGH);
  distance = duration / 29 / 2;
  return distance;
}
// ฟังก์ชันสแกนเนอร์
void fun_scnaner() {
  if (StartScan) { //status
    if (mySerial.available()) { //รับค่า
      char inChar = (char)mySerial.read();
      Student_Card_Str += inChar;
      if (inChar == '\n') {
        Student_Card_Str.toCharArray(Student_Card_Chr, 30);
        digitalWrite(PINOpenScanner, HIGH);
        tid.setText(Student_Card_Chr);
        CooldownP0 = true;
        StartScan = false;
      }
    }
  }
}
void loop() {
  // จอจับเวลา
  unsigned long currentMillis = millis(); //จำนวนรอบ หน่วยเป็นมินลิวินาที
  if (CooldownP0 == true) {
    CooldownP0 = false;
    backMillis = currentMillis;
    dimMillis = 0;
  } else {
    if (currentMillis - backMillis >= 60000 && backMillis != 0) {//back Home Page
      dimMillis = currentMillis;
      backMillis = 0;
      tid.setText("");
      GoTOPage0.show();
      Start_AGM = "wait";
      StartScan = false;
    } else if (currentMillis - dimMillis >= 60000 && dimMillis != 0) {
      dimMillis = 0;
      static String dim = "dim=70";
      sendCommand(dim.c_str()); // นำคำสั่งที่ต้องการเขียนโปรแกรมจอ ส่งไปที่จอ
    }
  }
  // เมื่อกดหน้าจอ Page0
  if (StartScan == true) {
    digitalWrite(PINOpenScanner, LOW); // ปิด Scanner
    delay(10);
    digitalWrite(PINOpenScanner, HIGH); // เปิด Scanner
  } else {
    digitalWrite(PINOpenScanner, HIGH);
  }
  //Scaner รับค่ารหัสนักศึกษา
  fun_scnaner();
  //Start AGM
  if (ProcessAGM) {
    Limitswitch();
    uint16_t lux = LightSensor.GetLightIntensity();
    if (Cycle == "open") {
      if (sw3State == 1) { //ฝาเปิด
        GoTOPage3.Show();
        servo_1.write(0);
        delay(500);
        Cycle = "close";
      }
    } else if (Cycle == "close") { //ฝาปิด
      if (lux <= 22  && sw3State == 1) {
        servo_1.write(200);
        delay(1500);
        Cycle = "process";
      }
    } else if (Cycle == "process") {
      int sensor = digitalRead(13);
      if (Cycle == "wait") { //รอรับค่าจาก sensor
        if (lux <= 26 && lux >= 10 && lux != 0 && sensor == 1) {  //พลาสติก
          MotorConTrol("R");
          Cycle = "plastic";
        } else if (lux >= 0 && lux <= 5 && sensor == 0) { //อลูมิเนียม
          MotorConTrol("L");
          Cycle = "can";
        } else if (lux >= 0 && lux <= 20 && sensor == 1) { //อื่น ๆ
          MotorConTrol("S");
          Cycle = "other";
        }
      } else { //ส่วนควบคุมกลไกคัดแยกขยะ
        if (Cycle == "other") {
          if (sw3State == 1) {
            MotorConTrol("S");
            servo_down();
            ProcessAGM = false;
          }
        } else if ( Cycle == "plastic") {
          if (sw1State == 1 && !chekst) {
            chekst = true;
            servo_down();
            MotorConTrol("L");
          } else if (sw3State == 1 && chekst) {
            MotorConTrol("S");
            chekst = false;
            ProcessAGM = false;
          }
        } else if (Cycle == "can") {
          if (sw2State == 1 && !chekst) {
            chekst = true;
            servo_down();
            MotorConTrol("R");
          } else if (sw3State == 1 && chekst) {
            MotorConTrol("S");
            chekst = false;
            ProcessAGM = false;
          }
        }

      }
      //Serial.println("SW1:" + String(sw1State) + " SW2:" + String(sw2State) + " SW3:" + String(sw3State));
      Serial.println("  Lux " + String(lux));
    }
  }
  else {
    if (WiFi.status() != WL_CONNECTED) {
      static unsigned long non_WiFi = 0;
      if (connecWIFI) {
        GoTOPage11.show();
      }
      else {
        GoTOPage10.show();
      }
      CooldownP0 = true;
      StartScan = false;
      backMillis = currentMillis - 59000;
      if (currentMillis - non_WiFi >= 1000 * 10) {
        connecWIFI = false;
        non_WiFi = currentMillis;
        WiFi.reconnect();
      }
    }
  } else {
    if (cm1 < 15) {
      cm1 = Ultrasonic(echoPIN1, trigPIN1);
      Serial.println(cm1);
      backMillis = currentMillis - 58000;
      if (line_check == false) {
        LINE.notify("ถังเต็ม(พลาสติก) ");
        line_check = true;
      }
    } else if (cm2 < 15) {
      cm2 = Ultrasonic(echoPIN2, trigPIN2);
      Serial.println(cm2);
      backMillis = currentMillis - 58000;
      if (line_check == false) {
        LINE.notify("ถังเต็ม(กระป๋อง) ");
        line_check = true;
      }
    } else if (cm3 < 15) {
      cm3 = Ultrasonic(echoPIN3, trigPIN3);
      Serial.println(cm3);
      backMillis = currentMillis - 58000;
      if (line_check == false) {
        LINE.notify("ถังเต็ม(อื่นๆ) ");
        line_check = true;
      }
    } else if (cm3 > 15 && cm2 > 15 && cm1 > 15) {
      line_check = false;
    }
  }
}
nexLoop(nex_listen_list);
}

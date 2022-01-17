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

  if (WiFi.status() != WL_CONNECTED) {
    if (Start_AGM == "wait") {
      static unsigned long non_WiFi = 0;
      GoTOPage10.show();
      CooldownP0 = true;
      StartScan = false;
      backMillis = currentMillis - 59000;
      if (currentMillis - non_WiFi >= 1000 * 10) {
        non_WiFi = currentMillis;
        WiFi.reconnect();
      }
    }
  } else {
    static uint8_t cm1, cm2, cm3;

    if (cm1 < 15) {
      cm1 = Ultrasonic(echoPIN2, trigPIN2);
      GoTOPage9.show();
      Serial.println(cm1);
      backMillis = currentMillis - 58000;
      //opop = false;
      if (line_check == false) {
        LINE.notify("ถังเต็ม(พลาสติก) ");
        line_check = true;
      }
    } else if (cm2 < 15) {
      cm2 = Ultrasonic(echoPIN2, trigPIN2);
      GoTOPage9.show();
      Serial.println(cm2);
      backMillis = currentMillis - 58000;
      //opop = false;
      if (line_check == false) {
        LINE.notify("ถังเต็ม(กระป๋อง) ");
        line_check = true;
      }
    } else if (cm3 < 15) {
      GoTOPage9.show();
      cm3 = Ultrasonic(echoPIN3, trigPIN3);
      Serial.println(cm3);
      backMillis = currentMillis - 58000;
      //opop = false;
      if (line_check == false) {
        LINE.notify("ถังเต็ม(อื่นๆ) ");
        line_check = true;
      }
    } else if (cm3 > 15 && cm2 > 15 && cm1 > 15) {
      line_check = false;
    }
  }
  //Scaner รับค่ารหัสนักศึกษา
  fun_scnaner();
  //Start AGM
  if (Start_AGM != "wait") {
    Limitswitch();
    uint16_t lux = LightSensor.GetLightIntensity();
    if (Start_AGM == "open") {
      if (sw3State == 1) { //ฝาเปิด
        servo_1.write(0);
        delay(500);
        Start_AGM = "close";
      }
    } else if (Start_AGM == "close") { //ฝาปิด
      if (lux <= 22  && sw3State == 1) {
        servo_1.write(200);
        delay(1500);
        Start_AGM = "start";
      }
    } else if (Start_AGM == "start") {
      int sensor = digitalRead(13);
      if (Cycle == "wait") { //รอรับค่าจาก sensor
        if (lux <= 26 && lux >= 10 && lux != 0 && sensor == 1) {  //พลาสติก
          stateControl = "R";
          Cycle = "plastic";
        } else if (lux >= 0 && lux <= 5 && sensor == 0) { //อลูมิเนียม
          stateControl = "L";
          Cycle = "can";
        } else if (lux >= 0 && lux <= 20 && sensor == 1) { //อื่น ๆ
          stateControl = "S";
          Cycle = "other";
        }
      } else { //ส่วนควบคุมกลไกคัดแยกขยะ
        if (Cycle == "other") {
          if (sw3State == 1) {
            stateControl = "S";
            servo_down();
            status_firebase = "other";
            Cycle = "wait";
            Start_AGM = "wait";
          }
        } else if ( Cycle == "plastic") {
          if (sw1State == 1 && !chekst) {
            chekst = true;
            servo_down();
            status_firebase = "plastic";
            stateControl = "L";
          } else if (sw3State == 1 && chekst) {
            stateControl = "S";
            chekst = false;
            Cycle = "wait";
            Start_AGM = "wait";
          }
        } else if (Cycle == "can") {
          if (sw2State == 1 && !chekst) {
            chekst = true;
            servo_down();
            
            status_firebase = "can";
            stateControl = "R";
          } else if (sw3State == 1 && chekst) {
            stateControl = "S";
            chekst = false;
            Cycle = "wait";
            Start_AGM = "wait";
          }
        }

      }
      //Motor control
      if (stateControl == "L") {
        MotorLeft();
      } else if (stateControl == "R") {
        MotorRight();
      } else {
        MotorStop();
      }
      //Serial.println("SW1:" + String(sw1State) + " SW2:" + String(sw2State) + " SW3:" + String(sw3State));
      Serial.println("  Lux " + String(lux));
    }
  }
  nexLoop(nex_listen_list);
}

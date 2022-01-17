void loop_two(void *pvParameters) {
  for (;;) {
    static int plasticGET_ALL = 0, otherGET_ALL = 0, canGET_ALL = 0;
    if (check) {
      check = false;
      GoTOPage6.show();
      StartScan = false;
      if (Student_Card_Str != "") {
        if (Student_Card_Str == Firebase.getString("Score/" + Student_Card_Str + "/ID")) {
          userLogin = true;
          plastic = Firebase.getInt("Score/" + Student_Card_Str + "/Point_Plastic");
          other = Firebase.getInt("Score/" + Student_Card_Str + "/Point_Other");
          can = Firebase.getInt("Score/" + Student_Card_Str + "/Point_Can");
          GoTOPage2.show();
          delay(100);
          tsid.setText(Student_Card_Str);
          nCan.setValue(can);
          nPlastic.setValue(plastic);
          nOther.setValue(other);
        } else {
          GoTOPage1.show();
          t0.setText("Error ID !");
          Student_Card_Str = "";
          StartScan = true;
        }
      }
      else {
        GoTOPage1.show();
        t0.setText("Not Found");
        StartScan = true;
      }
    }

    if (Cycle == "other") {
      Cycle = "";
      GoTOPageother.show();
      backMillis = 0; // ใส่เพื่อรอเวลาการทำงานของ  firebase
      if (userLogin) {
        other = other + 1;
        Firebase.setInt("Score/" + Student_Card_Str + "/Point_Other", other);
      }
      delay(1000);
      CooldownP0 = true;
      otherGET_ALL = Firebase.getInt("GarbageAll/Other");
      otherGET_ALL++;
      Firebase.setInt("GarbageAll/Other" , otherGET_ALL);
      cm3 = Ultrasonic(echoPIN1, trigPIN1);
      if (cm3 < 15) {
        GoTOPage9.show();
      }
      else {
        GoTOPage4.show();
      }
    }
    else if (Cycle == "plastic") {
      Cycle = "";
      GoTOPageplastic.show();
      backMillis = 0;
      if (userLogin) {
        plastic = plastic + 1;
        Firebase.setInt("Score/" + Student_Card_Str + "/Point_Plastic", plastic);
      }
      delay(1000);
      CooldownP0 = true;
      plasticGET_ALL = Firebase.getInt("GarbageAll/Plastic");
      plasticGET_ALL++;
      Firebase.setInt("GarbageAll/Plastic" , plasticGET_ALL);
      cm1 = Ultrasonic(echoPIN1, trigPIN1);
      if (cm1 < 15) {
        GoTOPage9.show();
      }
      else {
        GoTOPage4.show();
      }
    }
    else if (Cycle == "can") {
      Cycle = "";
      GoTOPagecan.show();
      backMillis = 0;
      if (userLogin) {
        can = can  + 1;
        Firebase.setInt("Score/" + Student_Card_Str + "/Point_Can", can);
      }
      delay(1000);
      CooldownP0 = true;
      GoTOPage4.show();
      canGET_ALL = Firebase.getInt("GarbageAll/Can");
      canGET_ALL++;
      Firebase.setInt("GarbageAll/Can" , canGET_ALL);
      cm2 = Ultrasonic(echoPIN1, trigPIN1);
      if (cm2 < 15) {
        GoTOPage9.show();
      }
      else {
        GoTOPage4.show();
      }
    }

    delay(1);
  }
}

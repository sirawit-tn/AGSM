void bPage0PopCallback (void *ptr)//กดหน้าแรกไปหน้า login
{
  CooldownP0 = true;
  StartScan = true;
  GoTOPage1.show();
  Student_Card_Str = "";
  tid.setText("");
  static String dim = "dim=100";
  sendCommand(dim.c_str());
}
void bstPopCallback (void *ptr) //เช็ค ID นักศึกษาบนจอ
{
  memset(buffer, 0, sizeof(buffer));
  tid.getText(buffer, sizeof(buffer));
  Student_Card_Str = buffer;
  Student_Card_Str.toCharArray(Student_Card_Chr, 30);
  CooldownP0 = true;
  check = true;
}
void bbPopCallback (void *ptr)//ย้อนกลับไปยังหน้า Login
{
  CooldownP0 = true;
  StartScan = true;
  GoTOPage1.show();
  Student_Card_Str = "";
}
void bnPopCallback (void *ptr)//ปุ่มทิ้ง
{
  CooldownP0 = true;
  ProcessAGM = true;
  Cycle = "open";
  GoTOPage3.show();
}
void byesPopCallback (void *ptr)//ทิ้งอีกรอบ
{
  CooldownP0 = true;
  ProcessAGM = true;
  Cycle = "open";
  GoTOPage3.show();
}
void bnouPopCallback (void *ptr)//ทิ้งแบบไม่เป็นสมาชิก
{
  CooldownP0 = true;
  ProcessAGM = true;
  Cycle = "open";
  userLogin = false;
}
void bwifiPopCallback (void *ptr)//ปุ่มเข้าระบบไวไฟ หน้าล็อคอิน
{
  memset(LoginWIFI, 0, sizeof(LoginWIFI));
  tgwifi.getText(LoginWIFI, sizeof(LoginWIFI));
  if (LoginWIFI == "GOTLIVERPOOL") {
    GoTOPage13.show();
  } else {
    GoTOPage0.show();
  }
}
void bnowifiPopCallback (void *ptr)//ปุ่มเข้าระบบไวไฟหน้า หน้าไม่เชื่อมต่อ
{
  memset(SSIDWIFI, 0, sizeof(SSIDWIFI));
  memset(PASSWIFI, 0, sizeof(PASSWIFI));
  t1.getText(SSIDWIFI, sizeof(SSIDWIFI));
  t2.getText(PASSWIFI, sizeof(PASSWIFI));
  connecWIFI = true;
  WiFi.begin(SSIDWIFI, PASSWIFI);
}

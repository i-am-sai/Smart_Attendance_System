#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>
#include "FPS_GT511C3.h"
#include "SoftwareSerial.h"
#include <LiquidCrystal.h>

hd44780_I2Cexp lcd;
hd44780_I2Cexp lcd2(0x27);

FPS_GT511C3 fps(4, 5);
SoftwareSerial ESP(0,1); // RX, TX

char *Name_List[] = {"Sai", "Shruti", "Vinit", "Utkarsh", "Syed", "Davana","Jyothi"};
String userName = ""; // Declare a global variable to store the user name

void setup()
{
  Serial.begin(9600);
  ESP.begin(9600);
  lcd.begin(16, 2);
  lcd.print("Smart Attendance");
  lcd.setCursor(0, 1);
  lcd.print("System");
  delay(2000);
  lcd.clear();
  fps.Open();
  fps.SetLED(true);
  pinMode(2, INPUT_PULLUP);
}

void Enroll()
{
  int enrollid = 0;
  bool usedid = true;
  while (usedid == true)
  {
    usedid = fps.CheckEnrolled(enrollid);
    if (usedid == true)
      enrollid++;
  }
  fps.EnrollStart(enrollid);
  lcd.clear();
  lcd.print("Enroll #");
  lcd.print(enrollid);
  while (fps.IsPressFinger() == false)
    delay(100);
  bool bret = fps.CaptureFinger(true);
  int iret = 0;
  if (bret != false)
  {
    lcd.clear();
    lcd.print("Remove finger");
    fps.Enroll1();
    while (fps.IsPressFinger() == true)
      delay(100);
    lcd.clear();
    lcd.print("Press again");
    while (fps.IsPressFinger() == false)
      delay(100);
    bret = fps.CaptureFinger(true);
    if (bret != false)
    {
      lcd.clear();
      lcd.print("Remove finger");
      fps.Enroll2();
      while (fps.IsPressFinger() == true)
        delay(100);
      lcd.clear();
      lcd.print("Press yet again");
      while (fps.IsPressFinger() == false)
        delay(100);
      bret = fps.CaptureFinger(true);
      if (bret != false)
      {
        lcd.clear();
        lcd.print("Remove finger");
        iret = fps.Enroll3();
        if (iret == 0)
        {
          lcd.clear();
          lcd.print("Enrolling Success");
          if (enrollid < 5)
          {
            lcd.setCursor(0, 1);
            lcd.print("Name: ");
            lcd.print(Name_List[enrollid]);
            userName = Name_List[enrollid]; // Store the user name in the global variable
            Serial.print("User Name: ");
            Serial.println(userName);
            // Send userName to ESP32
            ESP.println(userName);
          }
        }
        else
        {
          lcd.clear();
          lcd.print("Enroll Failed:");
          lcd.print(iret);
        }
      }
      else
        lcd.print("Failed 1");
    }
    else
      lcd.print("Failed 2");
  }
  else
    lcd.print("Failed 3");
}

void loop()
{
  
delay(1500);
  if (digitalRead(2) == 0)
  {
    Enroll();
  }
  if (fps.IsPressFinger())
  {
    fps.CaptureFinger(false);
    int id = fps.Identify1_N();
    lcd.clear();
    Serial.println(Name_List[id]);
    lcd.print("Welcome: ");
    if (id == 200)
    {
      lcd.print("Unknown");
    }
    else if (id < 5)
    {
     
      lcd.print(Name_List[id]);
      userName = Name_List[id]; // Store the user name in the global variable
      Serial.print("User Name: ");
      Serial.println(userName);
    }
    else
    {
      lcd.print("Invalid ID");
      delay(1000);
    }
  }
  else
  {
    lcd.clear();
    lcd.print("Hi!.....");
  }

  delay(1000);
}

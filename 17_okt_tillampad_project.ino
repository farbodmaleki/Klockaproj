/*
* Name: clock and temp project
* Author: Victor Huke
* Date: 2024-10-10
* Description: This project uses a ds3231 to measure time and displays the time to an 1306 oled display,
* Further, it measures temprature with a analog temprature module and displays a mapped value to a 9g-servo-motor
*/

// Include Libraries
#include <RTClib.h>
#include <Wire.h>
#include "U8glib.h"
#include <Servo.h>

// Init constants
const int tempPin = A1;

// Init global variables
char t[32];

// construct objects
RTC_DS3231 rtc;
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);    // Display which does not send AC
Servo myservo;

void setup() {
  // init communication
  Serial.begin(9600);
  Wire.begin();

  // Init Hardware
  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  u8g.setFont(u8g_font_unifont);
  myservo.attach(9);
}

void loop() {
  // Get time and temperature
  String currentTime = getTime();
  float currentTemp = getTemp();

  // Print to Serial
  Serial.println("The time is " + currentTime);
  Serial.println("The temperature is: " + String(currentTemp) + " °C");

  // Update OLED display
  oledWrite(currentTime, currentTemp);

  // Update servo position based on temperature
  servoWrite(currentTemp);

  delay(1000);
}

/*
*This function reads time from an ds3231 module and package the time as a String
*Parameters: Void
*Returns: time in hh:mm:ss as String
*/
String getTime() {
  DateTime now = rtc.now();
  return (String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()));
}

/*
* This function reads an analog pin connected to an analog temperature sensor and calculates the corresponding temp
*Parameters: Void
*Returns: temperature as float
*/
float getTemp() {
  float temp = 0;
  float R1 = 10000;  // value of R1 on board
  float logR2, R2, T;
  float c1 = 0.001129148, c2 = 0.000234125, c3 = 0.0000000876741;  // steinhart-hart coefficients for thermistor

  int Vo = analogRead(tempPin);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);  // calculate resistance on thermistor
  logR2 = log(R2);
  temp = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));  // temperature in Kelvin
  temp = temp - 273.15;  // convert Kelvin to Celsius

  return temp;
}

/*
* This function takes time and temperature and draws them to an OLED display
*Parameters: - time: String to write to display (current time)
*             - temp: float to write to display (current temperature)
*Returns: void
*/
void oledWrite(String time, float temp) {
  u8g.firstPage();

  do {
    u8g.drawStr(0, 20, ("Time: " + time).c_str());
    u8g.drawStr(0, 40, ("Temp: " + String(temp, 1) + " C").c_str());
  } while (u8g.nextPage());
}

/*
* takes a temperature value and maps it to corresponding degree on a servo
*Parameters: - value: temperature
*Returns: void
*/
void servoWrite(float value) {
  myservo.write(map(value, 20, 30, 0, 179));
}

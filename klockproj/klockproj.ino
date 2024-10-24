/*
* Name: klocka project
* Author: Farbod Maleki Ghalehabdolreza
* Date: 2024-10-10
* Description: This project uses a ds3231 to measure time and displays the time to an 1306 oled display,
* Further, it measures temprature with a analog temprature module and displays a mapped value to a 9g-servo-motor
*/

// Include Libraries
#include <RTClib.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include "U8glib.h"
#include <Servo.h>

// Init constants
const int tempPin = A1;

// Init global variables
#define NUMPIXELS 24
#define PIN 6
float temp = 0;
int delayval = 5;  // timing delay for ledring

char t[32];

// construct objects
RTC_DS3231 rtc;
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);  // Display which does not send AC
Servo myservo;
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  // init communication
  Serial.begin(9600);
  Wire.begin();

  // Init Hardware
  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  u8g.setFont(u8g_font_unifont);
  myservo.attach(9);
  pixels.begin();
  pixels.setBrightness(25);
  pixels.show();
}

void loop() {
  
  Serial.println("The time is " + getTime());
  delay(100);
  oledWrite(getTime());
  servoWrite(getTemp());
  Serial.println(getTemp());
  delay(100);

  // LED ring behavior based on temperature
  if (getTemp() <= 24) {
    ledringBlue();
  } 
  else if (getTemp() >= 30) {
    ledringRed();
  }
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
* This function reads an analog pin connected to an analog temprature sensor and calculates the corresponding temp
*Parameters: Void
*Returns: temprature as float
*/
float getTemp() {
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
* This function takes a string and draws it to an oled display
*Parameters: - text: String to write to display
*Returns: void
*/
void oledWrite(String text) {
  u8g.firstPage();
  do {
    u8g.drawStr(0, 20, text.c_str());
    u8g.setPrintPos(0, 40);
    u8g.print(temp);
  } while (u8g.nextPage());
}

/*
* Takes a temperature value and maps it to corresponding degree on a servo
*Parameters: - value: temperature
*Returns: void
*/
void servoWrite(float value) {
  myservo.write(map(value, 20, 30, 0, 179));
}

/*
* Set the LED ring to red color for temperatures >= 30°C
*Parameters: None
*Returns: void
*/
void ledringRed() {
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(255, 0, 0));  // Red color
  }
  pixels.show();  // Send updated colors to the LED ring
}

/*
* Set the LED ring to blue color for temperatures <= 24°C
*Parameters: None
*Returns: void
*/
void ledringBlue() {
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 255));  // Blue color
  }
  pixels.show();  // Send updated colors to the LED ring
}

/*
* Name: klocka project
* Author: Farbod Maleki Ghalehabdolreza
* Date: 2024-10-10
* Description: This project uses a ds3231 to measure time and displays the time to an 1306 oled display,
* Further, it measures temprature with a analog temprature module and displays a mapped value to a 9g-servo-motor
*/

// Include Libraries
#include <RTClib.h> // Bibliotek för RTC (Real Time Clock) DS3231
#include <Adafruit_NeoPixel.h> // Bibliotek för att styra NeoPixel LED-ringen
#include <Wire.h>  //I2C-kommunikationsbibliotek
#include "U8glib.h" // Bibliotek för att styra OLED-skärmen
#include <Servo.h> // Bibliotek för att styra servomotorn

// Init constants
const int tempPin = A1; // Analog pin för att läsa temperaturen från sensorn

// Init global variables
#define NUMPIXELS 24  // Antal LEDs i NeoPixel-ringen
#define PIN 6 // Digital pin där NeoPixel-ringen är ansluten
float temp = 0; // Variabel för att lagra temperaturen
int delayval = 5;  // Fördröjningstid för LED-ringens ljussekvens

char t[32];  // Strängbuffer för att lagra tid som ska visas

// construct objects
RTC_DS3231 rtc;  // Skapa ett objekt för RTC (Real Time Clock)
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);  // Skapa objekt för OLED-displayen
Servo myservo;  // Skapa ett servo-objekt
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);  // Skapa objekt för NeoPixel LED-ringen

void setup() {
  // init communication
  Serial.begin(9600); // Starta seriell kommunikation med baud rate 9600
  Wire.begin(); // Starta I2C-kommunikation

  // Init Hardware
  rtc.begin();  // Starta RTC
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); 
  u8g.setFont(u8g_font_unifont);  // Ställ in teckensnittet för OLED-skärmen
  myservo.attach(9); // Anslut servomotorn till digital pin 9
  pixels.begin(); // Initiera NeoPixel-ringen
  pixels.setBrightness(25); // Ställ in ljusstyrkan på LED-ringen till 25%
  pixels.show(); // Uppdatera LED-ringen med den inställda ljusstyrkan
}

void loop() {
  
  Serial.println("The time is " + getTime());   // Hämta och skriv ut tiden
  delay(100);
  oledWrite(getTime()); // Skriv tiden på OLED-displayen
  servoWrite(getTemp()); // Ställ servomotorn baserat på aktuell temperatur
  Serial.println(getTemp()); // Skriv ut temperaturen i seriell monitor
  delay(100);

  // LED ring behavior based on temperature
  if (getTemp() <= 24) { // Om temperaturen är mindre än eller lika med 24 grader
    ledringBlue();  // Tänd LED-ringen i blått
  } 
  else if (getTemp() >= 30) {  // Om temperaturen är större än eller lika med 30 grader
    ledringRed(); // Tänd LED-ringen i rött
  }
}


/*
*This function reads time from an ds3231 module and package the time as a String
*Parameters: Void
*Returns: time in hh:mm:ss as String
*/
String getTime() {
  DateTime now = rtc.now();  // Hämta aktuell tid från RTC-modulen
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
  u8g.firstPage(); // Starta den första sidan av uppdateringscykeln för OLED-displayen
  do {
    u8g.drawStr(0, 20, text.c_str());  // Rita strängen på position (0, 20) på OLED-skärmen
    u8g.setPrintPos(0, 40);  // Ställ in positionen för att skriva ut temperatur
    u8g.print(temp);  // Skriv ut den aktuella temperaturen på displayen
  } while (u8g.nextPage());  // Gå till nästa sida tills alla sidor är ritade alltså while
}

/*
* Takes a temperature value and maps it to corresponding degree on a servo
*Parameters: - value: temperature
*Returns: void
*/
void servoWrite(float value) {
  myservo.write(map(value, 20, 30, 0, 179));  // Mappa temperaturvärdet till en vinkel mellan 0 och 179 grader för servon
}

/*
* Set the LED ring to red color for temperatures >= 30°C
*Parameters: None
*Returns: void
*/
void ledringRed() {
  for (int i = 0; i < NUMPIXELS; i++) {  // Loop för att sätta färg på varje LED i ringen
    pixels.setPixelColor(i, pixels.Color(255, 0, 0));   // Ställ in aktuell LED till röd färg
  }
  pixels.show();  // Skicka de uppdaterade färgerna till LED-ringen
}

/*
* Set the LED ring to blue color for temperatures <= 24°C
*Parameters: None
*Returns: void
*/
void ledringBlue() {
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 255));  // Loop för att sätta färg på varje LED i ringen
  }
  pixels.show();  // Skicka de uppdaterade färgerna till LED-ringen bkkjjkkuhuijik
}

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <RTClib.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

//Oled init
#define OLED_RESET     4
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//Sensor init
int moisture_sensor = A0;
float soil_moisture;

//Alert led init
int pump = 3;

//Clock init
RTC_DS3231 rtc;

void setup() {
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
   if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  }
  
  pinMode(pump, OUTPUT);
  Serial.begin(9600);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  

}

void loop() {

 
  //Read soil moisture
  soil_moisture = analogRead(moisture_sensor);
  //soil_moisture = map(soil_moisture, 674, 155, 0, 100);

  //Time format
  DateTime now = rtc.now();
  char time[5];
  sprintf(time, "%02d:%02d",  now.hour(), now.minute());

  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print(time);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,25); 
  display.print("Soil moisture:" );
  display.setCursor(90,25); 
  display.print(soil_moisture);
  display.setCursor(0,35);
  display.print("Temperature:" );
  display.setCursor(90,35);
  display.print(rtc.getTemperature());
  display.display();
  digitalWrite(pump, LOW);
  delay(1000);
  
  

  if (now.minute() % 20 == 0 && now.second() == 0) {
    Serial.print(time);
    Serial.print("-");
    Serial.print(soil_moisture);
    Serial.print("\n");
  }

  if (now.minute() == 0 && (now.hour() == 16 || now.hour() == 4) && now.second() < 15) {
    Serial.print(time);
    Serial.print("-");
    Serial.print(soil_moisture);
    Serial.print("\n");

    if (soil_moisture < 170) {
      digitalWrite(pump, LOW);
    }

  }
  else {
    digitalWrite(pump, HIGH);
  }

}

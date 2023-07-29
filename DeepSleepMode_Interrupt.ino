#include <U8x8lib.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>


#define uS_TO_S_FACTOR 1000000
#define TEMPO_DEBOUNCE 10


TinyGPSPlus gps;

U8X8_SSD1306_128X64_NONAME_SW_I2C display(/* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // OLEDs without Reset of the Display

HardwareSerial neogps(1);

RTC_DATA_ATTR int bootCount = 0;
unsigned long timestamp_ultimo_acionamento = 0;

void configureDisplay(){
  display.begin();
  display.setPowerSave(0);
  display.setFont(u8x8_font_torussansbold8_u);

  display.drawString(0, 0, "BOOT NUMERO: ");
  display.drawString(13, 0, String(bootCount).c_str());
  


}

void countToSleep(int secondsToSleep, int secondsSleep){
  display.drawString(0, 4, "DORMINDO EM:");
  display.drawString(0, 6, "              ");
  for(int i = secondsToSleep; i >= 0; i--){
    if(i < 10){
      display.drawString(0, 6, "0");
      display.drawString(1, 6, String(i).c_str());
    }
    if(i >= 10){
      display.drawString(0, 6, String(i).c_str());
    }
    display.drawString(3, 6, " SEGUNDOS");
    
    delay(1000);
  }
  display.drawString(0, 4, "DORMIU...     ");
  display.drawString(0, 6, "ZZZZZZZZZZ...");
  esp_sleep_enable_timer_wakeup(secondsSleep * uS_TO_S_FACTOR);
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_33,1); //1 = High, 0 = Low
  Serial.println("Dormindo...");
  esp_deep_sleep_start();
}



void setup() {
  Serial.begin(115200);
  neogps.begin(9600, SERIAL_8N1,16,17);
  delay(1000);
  ++bootCount;
  pinMode(4, INPUT);
  Serial.println("Numero do Boot: " + String(bootCount));
  configureDisplay();
  //countToSleep(10, 20);

}

void loop() {
  if(digitalRead(4)){
    countToSleep(10, 10);
  }
  while (neogps.available() > 0)
    if (gps.encode(Serial2.read()))
      displayInfo();

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }
}

void displayInfo()
{
  Serial.print(F("Location: ")); 
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();
}


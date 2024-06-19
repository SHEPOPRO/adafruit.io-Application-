#include <Adafruit_Sensor.h>
#include <ESP8266WiFi.h>
#include "config.h"

const char* Adafrui_TOKEN = "xxxxxxxxxx";  
const char* WIFI_SSID = "xxxxxxxxx";                                    
const char* WIFI_PASS = "xxxxxxx"; 
#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10
#define LED_PIN 13
#define FAN_PIN 15
#define DOOR_PIN A3
#define BUZZER_PIN 12 
#define ALARM_PIN 5

//set the feeds
  AdafruitIO_Feed *TempFeed = io.feed("temperature");
  AdafruitIO_Feed *HumFeed = io.feed("humidity");
  AdafruitIO_Feed *Digital = io.feed("digital");
  AdafruitIO_Feed *Alarm = io.feed("alarm");
  AdafruitIO_Feed *Door = io.feed("door");

//global variable
  bool isAlarm = false;
  bool isDoor = false;

  void handleMessage(AdafruitIO_Data *data) {

  Serial.print("received <- ");

  if(data->toPinLevel() == HIGH)
    Serial.println("HIGH");
  else
    Serial.println("LOW");
  digitalWrite(LED_PIN, data->toPinLevel());
}

void handleAlarm(AdafruitIO_Data *data) {
// handle the alarm toggle on the Adafruit IO Dashboard
  String toggleValue = data->toString();
  Serial.print("> rcv alarm: ");
  Serial.println(toggleValue);
  if(toggleValue == String("ON")) {
    Serial.println("* Alarm Set: ON");
    isAlarm = true;
  } else {
    Serial.println("* Alarm Set: OFF");
    isAlarm = false;
  }
}

void setup() {
//set inputs/outputs
    pinMode(LED_PIN, OUTPUT);
    pinMode(FAN_PIN, OUTPUT);
    pinMode(DOOR_PIN, INPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(ALARM_PIN, OUTPUT);
 
  Serial.begin(9600);
    while (!Serial);
  Serial.println(F("BME680 test"));
    if (!bme.begin()) {
     Serial.println("Could not find a valid BME680 sensor, check wiring!");
        while (1);
    }
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);

  Serial.print("Connecting to Adafruit IO");
  io.connect();
  // wait for a connection
    while (io.status() < AIO_CONNECTED)
   {
    Serial.print(".");
    delay(500);
   }
 
  // we are connected
  Serial.println();
  Serial.println(io.statusText());
  Digital->onMessage(handleMessage);
  Alarm->onMessage(handleAlarm);
  Digital->get();
  Alarm->get();
 
}

void loop() {
  io.run();

  Serial.println("Reading Sensors....");
  
  int TempRead = bme.readTemperature();
  TempRead=((TempRead)*(9/5))+32; 
  Serial.print("Temperature = "); 
  Serial.print(bme.temperature);
  Serial.println("*C");

  int HumRead = bme.readHumidity();
  Serial.print("Humidity = ");
  Serial.print(bme.humidity);
  Serial.println("%");

//saves to the io
  TempFeed->save(TempRead);
  HumFeed->save(HumRead);
  
// Fan turns on if it gets above 70 degrees 
  if(TempRead >= 70){
    digitalWrite(15,HIGH);
    }
  else
    digitalWrite(15,LOW);
int i = 0;    
//Alarm system: if door is open and the alarm is set then turn on the buzzer

  if(analogRead(DOOR_PIN)>0){ 
    isDoor = true;
      if(isAlarm ==true)
        digitalWrite(BUZZER_PIN,HIGH);
  }
  else{
      digitalWrite(BUZZER_PIN,LOW);
      isDoor=false;
      }
   Door->save(isDoor);
//wait 20 sec to check sensors again
   delay(2500);

}

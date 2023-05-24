#include <Adafruit_MAX31865.h>
#include "DHT.h"  //library sensor yang telah diimportkan
#include <SoftwareSerial.h>
#include <MQ2.h>
#include <Wire.h>



// The value of the Rref resistor. Use 430.0 for PT100 and 4300.0 for PT1000
#define RREF 430.0
// The 'nominal' 0-degrees-C resistance of the sensor
// 100.0 for PT100, 1000.0 for PT1000
#define RNOMINAL 100.0
#define DHTPIN 9       //Pin apa yang digunakan
#define DHTTYPE DHT11  // DHT 11
Adafruit_MAX31865 thermo = Adafruit_MAX31865(10, 11, 12, 13);
// use hardware SPI, just pass in the CS pin
//Adafruit_MAX31865 thermo = Adafruit_MAX31865(10);
SoftwareSerial espSerial(5, 6);
int dt = 0;
int buzzer = 4;
int pinAout = A1;
char buff[100];
float lpg_gas, co_gas, smoke_gas;
volatile byte index;
volatile bool receivedone;


MQ2 mq2(pinAout);

DHT dht(DHTPIN, DHTTYPE);
void receiveEvent(int howMany) {
  while (1 < Wire.available()) {
  }
  char x = (char)Wire.read();  // receive byte as an integer
  String data;
  data += x;
  dt = data.toInt();
  Serial.println(dt);  // print the integer
  if (dt == 1) {
    digitalWrite(buzzer, LOW);
  } else if (dt == 2) {
    digitalWrite(buzzer, HIGH);
  }
}

void setup() {
  Serial.println("Adafruit MAX31865 PT100 Sensor Test!");
  Wire.begin(8);  // join i2c bus with address #4
  Wire.onReceive(receiveEvent);
  Serial.begin(9600);  //baud komunikasi serial
  thermo.begin(MAX31865_3WIRE);
  espSerial.begin(115200);
  pinMode(buzzer, OUTPUT);
  Serial.println("Pengujian DHT11! dan sensor gas");  //penulisan di serial monitor
  dht.begin();                                        //prosedur memulai pembacaan module sensor
  mq2.begin();
}

void loop() {
  delay(2000);  //menunggu beberapa detik untuk pembacaan

  //pembacaan sensor membutuhkan waktu 250ms

  //Pembacaan untuk data kelembaban
  float humidity_1 = dht.readHumidity();
  //Pembacaan dalam format celcius (c)
  float celcius_1 = dht.readTemperature();
  //pembacaan dalam format Fahrenheit
  float fahrenheit_1 = dht.readTemperature(true);

  //mengecek pembacaan apakah terjadi kegagalan atau tidak
  if (isnan(humidity_1) || isnan(celcius_1) || isnan(fahrenheit_1)) {
    Serial.println("Pembacaan data dari module sensor gagal!");
    return;
  }

  float htof = dht.computeHeatIndex(fahrenheit_1, humidity_1);
  //Prosedur pembacaaan data indeks panas dalam bentuk fahreheit
  float htoc = dht.computeHeatIndex(celcius_1, humidity_1, false);
  //Prosedur pembacaaan data indeks panas dalam bentuk celcius
  float* values = mq2.read(true);  //jika diset "false" tidak akan dimunculkan di serial monitor


  //pembacaan nilai pembacaan data kelembaban
  Serial.print("Kelembaban: ");
  Serial.print(humidity_1);
  Serial.print(" %\t");

  //pembacaan nilai pembacaan data suhu
  Serial.print("Suhu : ");
  Serial.print(celcius_1);  //format derajat celcius
  Serial.print("°");        //simbol derajat
  Serial.print("C / ");
  Serial.print(fahrenheit_1);  //format derajat fahrenheit
  Serial.print("°");
  Serial.print("F\t");

  Serial.print("Indeks Panas: ");
  Serial.print(htof);
  Serial.print("°");
  Serial.print("F / ");
  Serial.print(htoc);
  Serial.print("°");
  Serial.println("C ");
  lpg_gas = mq2.readLPG();
  co_gas = mq2.readCO();
  smoke_gas = mq2.readSmoke();
  String str = ('#') + String(humidity_1) + (',') + String(celcius_1) + (',') + String(fahrenheit_1) + (',') + String(lpg_gas) + String(',') + String(co_gas) + String(',') + String(smoke_gas);
  espSerial.println(str);
  uint16_t rtd = max.readRTD();
  
  Serial.print("RTD value: "); Serial.println(rtd);
  float ratio = rtd;
  ratio /= 32768;
  Serial.print("Ratio = "); Serial.println(ratio,8);
  Serial.print("Resistance = "); Serial.println(RREF*ratio,8);
  Serial.print("Temperature = "); Serial.println(max.temperature(RNOMINAL, RREF));

  // Check and print any faults
  uint8_t fault = thermo.readFault();
  if (fault) {
    Serial.print("Fault 0x");
    Serial.println(fault, HEX);
    if (fault & MAX31865_FAULT_HIGHTHRESH) {
      Serial.println("RTD High Threshold");
    }
    if (fault & MAX31865_FAULT_LOWTHRESH) {
      Serial.println("RTD Low Threshold");
    }
    if (fault & MAX31865_FAULT_REFINLOW) {
      Serial.println("REFIN- > 0.85 x Bias");
    }
    if (fault & MAX31865_FAULT_REFINHIGH) {
      Serial.println("REFIN- < 0.85 x Bias - FORCE- open");
    }
    if (fault & MAX31865_FAULT_RTDINLOW) {
      Serial.println("RTDIN- < 0.85 x Bias - FORCE- open");
    }
    if (fault & MAX31865_FAULT_OVUV) {
      Serial.println("Under/Over voltage");
    }
    thermo.clearFault();
  }
  Serial.println();

  delay(1000);
}

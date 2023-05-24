#include "DHT.h" //library sensor yang telah diimportkan
#include <SoftwareSerial.h>
#include <MQ2.h>
#include <Wire.h>



#define DHTPIN 9     //Pin apa yang digunakan
#define DHTTYPE DHT11   // DHT 11
SoftwareSerial espSerial(5, 6);
int dt = 0;
int buzzer = 4;
int pinAout = A1;
char buff [100];
float lpg_gas, co_gas, smoke_gas;
volatile byte index;
volatile bool receivedone;

 
MQ2 mq2(pinAout);
 
DHT dht(DHTPIN, DHTTYPE);
void receiveEvent(int howMany) {
  while(1 < Wire.available()) { 
  }
  char x = (char)Wire.read();          // receive byte as an integer
  String data;
  data += x;
  dt = data.toInt();
  Serial.println(dt);            // print the integer
  if ( dt == 1 )
  {
    digitalWrite(buzzer, LOW);
  }
  else if (dt == 2)
  {
  digitalWrite(buzzer, HIGH);
  }
}
 
void setup() {
  Wire.begin(8);                // join i2c bus with address #4
  Wire.onReceive(receiveEvent);
  Serial.begin(9600); //baud komunikasi serial
  espSerial.begin(115200);
  pinMode(buzzer, OUTPUT);
  Serial.println("Pengujian DHT11! dan sensor gas"); //penulisan di serial monitor
  dht.begin(); //prosedur memulai pembacaan module sensor
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
  float* values= mq2.read(true); //jika diset "false" tidak akan dimunculkan di serial monitor
  
 
  //pembacaan nilai pembacaan data kelembaban
  Serial.print("Kelembaban: ");
  Serial.print(humidity_1);
  Serial.print(" %\t");
  
  //pembacaan nilai pembacaan data suhu
  Serial.print("Suhu : ");
  Serial.print(celcius_1); //format derajat celcius
  Serial.print("°"); //simbol derajat
  Serial.print("C / ");
  Serial.print(fahrenheit_1); //format derajat fahrenheit
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
  String str = ('#')+String(humidity_1)+(',')+String(celcius_1)+(',')+String(fahrenheit_1)+(',')+String(lpg_gas)+String(',')+String(co_gas)+String(',')+String(smoke_gas);
  espSerial.println(str);
 
  
delay(1000);
}

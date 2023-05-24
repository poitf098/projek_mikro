#include <WiFi.h>  
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <Wire.h>
//---- pengaturan wifi
const char* ssid = "Surya";
const char* password = "dodik123456";

//---- MQTT Broker setting
const char* mqtt_server = "830f9d7f1fe74e01af34f37486f23891.s1.eu.hivemq.cloud"; // replace with your broker url
const char* mqtt_username = "surya";
const char* mqtt_password = "surya123";
const int mqtt_port =8883;

 

WiFiClientSecure espClient;   // for no secure connection use WiFiClient instead of WiFiClientSecure 
//WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];


float data = 0;
float data1 = 0;
float data2 = 0;
float data3 = 0;
float data4 = 0;
float data5 = 0;
int command1 =0;
int BUILTIN_LED = 2;
String dataIn;
String dt[15];
int i;
boolean parsing=false;

const char* datatopic= "data";
const char* data1topic="data1";
const char* data2topic="data2";
const char* data3topic="data3";
const char* data4topic="data4";
const char* data5topic="data5";;
//const char*  sensor2_topic="sensor3";

const char* command1topic="command1";
//const char* command1_topic="command2";





static const char *root_ca PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
)EOF";
void parsingData(){
int j=0;
 
//kirim data yang telah diterima sebelumnya
Serial.print("\n data masuk : \n");
Serial.print(dataIn);
 
//inisialisasi variabel, (reset isi variabel)
dt[j]="";
//proses parsing data
for(i=1;i<dataIn.length();i++){
//pengecekan tiap karakter dengan karakter (#) dan (,)
if ((dataIn[i] == '#') || (dataIn[i] == ','))
{
//increment variabel j, digunakan untuk merubah index array penampung
j++;
dt[j]="";       //inisialisasi variabel array dt[j]
}
else
{
//proses tampung data saat pengecekan karakter selesai.
dt[j] = dt[j] + dataIn[i];
}
}
 
//kirim data hasil parsing
Serial.print("data 1 : ");
Serial.print("\n Kelembaban: "+dt[0]+"% \n");
Serial.print("data 2 : ");
Serial.print("\n Suhu : "+dt[1]+"°C \n");
Serial.print("data 3 : ");
Serial.print("\n Suhu : "+dt[2]+"°F \n");
Serial.print("data 4 : ");
Serial.print("\n gas lpg : "+dt[3]+" ppm \n");
Serial.print("data 5 : ");
Serial.print("\n gas co : "+dt[4]+" ppm \n");
Serial.print("data 6 : ");
Serial.print("\n smoke : "+dt[5]+" ppm \n");
Serial.print("data dari arduino\n\n");
}

void kirim(){
data = dt[0].toFloat();
data1 = dt[1].toFloat();
data2 = dt[2].toFloat();
data3 = dt[3].toFloat();
data4 = dt[4].toFloat();
data5 = dt[5].toFloat();
}
void setup_wifi() {
  delay(10);
  Serial.print("\nConnecting to \n");
  Serial.print(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  randomSeed(micros());
  Serial.print("\n WiFi connected\nIP address: \n");
  Serial.print(WiFi.localIP());
}


//=====================================
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32Client-";   // Create a random client ID
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      Serial.print("\n connected");

      client.subscribe(command1topic);   // subscribe the topics here
      //client.subscribe(command2_topic);   // subscribe the topics here
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.print(" try again in 5 seconds");   // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
   Serial.begin(115200);
   Wire.begin(21, 22);   
   dataIn="";
   while (!Serial) delay(1);
   setup_wifi();
   pinMode(BUILTIN_LED, OUTPUT);
   espClient.setCACert(root_ca);
   client.setServer(mqtt_server, mqtt_port);
   client.setCallback(callback);
}

void loop() {
  if(Serial.available()>0) {
    delay(10);
    char inChar = (char)Serial.read();
    dataIn += inChar;
    if (inChar == '\n') {
    parsing = true;
  }  
}
 
if(parsing){
    parsingData();
    parsing=false;
    dataIn="";
  }
  unsigned long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
    kirim();
  if (!client.connected()) reconnect();
  client.loop();
    publishMessage(datatopic,String(data),true);    
    publishMessage(data1topic,String(data1),true);
    publishMessage(data2topic,String(data2),true);    
    publishMessage(data3topic,String(data3),true);
    publishMessage(data4topic,String(data4),true);    
    publishMessage(data5topic,String(data5),true);
}
}
void callback(char* topic, byte* payload, unsigned int length) {
  String incommingMessage = "";
  for (int i = 0; i < length; i++) incommingMessage+=(char)payload[i];
  
  Serial.print("Message arrived ["+String(topic)+"]"+incommingMessage);
  
  //--- check the incomming message
    if( strcmp(topic,command1topic) == 0){
     if (incommingMessage.equals("1")) 
       {
       digitalWrite(BUILTIN_LED, LOW);   // Turn the LED off
       Wire.beginTransmission(8);
       Wire.write("1");
       Wire.endTransmission();
       }
     else 
     {
       digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED on
       Wire.beginTransmission(8);
       Wire.write("2");
       Wire.endTransmission();

     }

  }
}

void publishMessage(const char* topic, String payload , boolean retained){
  if (client.publish(topic, payload.c_str(), true))
      Serial.print("Message publised ["+String(topic)+"]: "+payload);
}

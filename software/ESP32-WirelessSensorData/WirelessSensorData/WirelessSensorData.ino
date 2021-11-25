#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <WiFiUDP.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1050)
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

const char *ssid = "yourAP";
const char *password = "1234567890";
const char *fallback_ssid = "NSIT-Campus";
const char *fallback_pwd = "";

boolean self_ap = false;
IPAddress myIP;
int myPort;
WiFiUDP udp;
char incomingPacket[256];
String reply_s;
char * reply;
char *myssid = "";

Adafruit_BME280 bme;
float temperature, humidity, pressure, altitude;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void initialize_display();
void display_info();
void service_client();
void get_readings();
void form_reply();

void setup()
{
  //Configure Communications
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  Serial.println("Scanning WiFI");
  WiFi.begin(fallback_ssid, fallback_pwd);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Self Hosting");
    self_ap = true;
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);
  }
  if(self_ap) {
    myIP = WiFi.softAPIP();
    myssid = (char *)ssid;
    myPort = 1234;
  }
  else {
    myIP = WiFi.localIP();
    myPort = 1234;
    myssid = (char *)fallback_ssid;
  }
  udp.begin(myPort);
  
  //Configure Sensors
  Serial.println("Configuring BME");
  bme.begin(0x76);
  delay(100);

  //Configure Display
  Serial.println("Configuring Display");
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);

  Serial.println();
  Serial.println("Configuring access point...");

  Serial.print("IP address: ");
  Serial.println(myIP);
  initialize_display();
  display_info();
}

void loop()
{
  get_readings();
  delay(1000);
  service_client();
}

void get_readings(){
  //temperature = bme.readTemperature();
  temperature++;
  Serial.print("Temp: ");
  Serial.println(temperature);
  humidity = bme.readHumidity();
  pressure = bme.readPressure() / 100.0F;
  altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
}

void initialize_display() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,20);
}

void display_info(){
  display.print("IP: ");
  display.println(myIP);
  display.print("\nPort: ");
  display.println(myPort);
  display.print("\nAP: ");
  display.println(myssid);
  display.display();
}

void form_reply(){
  reply_s = String("Temperature: ");
  reply_s += String(temperature,2) + String("\nHumidity: ") + String(humidity,2) + String("\nPressure: ") + String(pressure,2) + String("\nAltitude: ") + String(altitude,2);
}

void service_client() {
  Serial.print("UDP Listening on IP: ");
  Serial.println(myIP);
  int packetSize = udp.parsePacket();
  if(packetSize){
    Serial.printf("Received %d bytes from %s, port %d\n", packetSize, udp.remoteIP().toString().c_str(), udp.remotePort());
    int len = udp.read(incomingPacket,6);
    if(len > 0) {
      Serial.printf("Packet Received: %0s\n", incomingPacket);
      udp.beginPacket(udp.remoteIP(), udp.remotePort());
      int i = 0 ;
      form_reply();
      while(reply_s.c_str()[i] != 0) {
        udp.write((uint8_t)reply_s.c_str()[i]);
        i++;
      }
      Serial.print("Endpacket");
      udp.endPacket();
    }
   }
}

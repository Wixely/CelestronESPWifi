#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "SoftwareSerial.h"
#include "WebServer.h"

/* Variables you can change */
const String ssid = "Celestron";
const String pw = "Celestron";

int RTSPin = 2;
int UartTXPin = 5;
int UartRXPin = 4;

/* Variables you should NOT change */
IPAddress hostIP(1, 2, 3, 4);
IPAddress subnetMask(255, 255, 255, 0);
WiFiServer tcpServer(2000);
SoftwareSerial serialTX;
SoftwareSerial serialRX;
WiFiClient tcpClient;
byte buffer[1024];
byte bufferByte;
int bufferSize = 0;

void setup() {
  pinMode(RTSPin, OUTPUT);
  digitalWrite(RTSPin, HIGH);
  delay(100);
  Serial.begin(19200);
  Serial.println("Starting");
  serialTX.begin(19200, SWSERIAL_8N1, UartTXPin, UartTXPin, false, 256); 
  serialRX.begin(19200, SWSERIAL_8N1, UartRXPin, UartRXPin, false, 256); 
  serialTX.enableIntTx(false);
  serialRX.enableIntTx(false);
  serialTX.enableTx(true);
  serialRX.enableTx(false);
  
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(hostIP, hostIP, subnetMask);
  WiFi.softAP(ssid, pw);
  StartupHostHTTPServer();
  
  tcpServer.begin();
  delay(100);
}

void loop() {
  while(!tcpClient.connected())
  {
    tcpClient = tcpServer.available();
    server.handleClient();
    //delay(10);
  }

  if (TransmitData(&tcpClient, &serialTX)) //send data from tcp client to scope
    delay(2); 

  TransmitData(&serialRX, &tcpClient); //send data from scope to tcp client
}

int TransmitData(WiFiClient* tcp, SoftwareSerial* uart)
{
  int sentData = 0;
  if (tcp->available())
  {
    Serial.print("TCP: ");
    while (tcp->available())
    {
      bufferByte = (byte)tcp->read(); // read the data and pipe it directly to the scope uart
      Serial.print(bufferByte < 0x10 ? PSTR(" 0") : PSTR(" "));
      Serial.print(bufferByte, HEX);
      buffer[bufferSize++] = bufferByte;
    }

    digitalWrite(RTSPin, LOW);
    uart->write(buffer, bufferSize);
    uart->flush();
    digitalWrite(RTSPin, HIGH);
    
    bufferSize = 0;
    sentData = 1;
    Serial.println();
  }
  return sentData;
}

int TransmitData(SoftwareSerial* uart, WiFiClient* tcp)
{
  int sentData = 0;
  if (uart->available())
  {
    Serial.print("UART: ");
    while (uart->available())
    {
      bufferByte = (byte)uart->read(); // read the data and pipe it directly to the tcp client
      Serial.print(bufferByte < 0x10 ? PSTR(" 0") : PSTR(" "));
      Serial.print(bufferByte, HEX);
      buffer[bufferSize++] = bufferByte;
    }
    tcp->write(buffer, bufferSize);
    bufferSize = 0;
    sentData = 1;
    Serial.println();
  }
  return sentData;
}

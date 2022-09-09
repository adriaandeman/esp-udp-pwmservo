#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

WiFiUDP Udp;

const char* ssid = "#####";
const char* password = "#####";

unsigned const int localUdpPort = 4210;

const char replyPacket[] = "ack/n"; 

const byte maxPins = 14;

char incomingPacket[255];

int pinValue[maxPins]  = {0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0, 0,  0};

void setup()
{
  Serial.begin(115200);
  delay(10000);
  Serial.println();

  Wire.pins(2, 0);
  pwm.begin();
  pwm.setPWMFreq(1000);

  Wire.setClock(400000);

  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("connected!");

  Udp.begin(localUdpPort);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);

}

void loop()
{
  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    int len = Udp.read(incomingPacket, 255);
    if (len > 0) incomingPacket[len] = 0;

    Serial.printf("UDP packet contents: %s\n", incomingPacket);

    sscanf (incomingPacket, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d",
            &pinValue[7], &pinValue[2], &pinValue[4], &pinValue[0], &pinValue[1], &pinValue[3],
            &pinValue[11], &pinValue[6], &pinValue[8], &pinValue[5], &pinValue[9], &pinValue[13],
            &pinValue[10], &pinValue[12]);

    if (pinValue[1] == 101) {
      Udp.beginPacket(Udp.remoteIP(), 1337);
      Udp.write(replyPacket);
      Udp.endPacket();
    }
    else {
      for (int i = 0; i < maxPins ; i++) {
        pwm.setPWM(i, 0, pinValue[i] * 41 );
        yield();
      }
    }
  }
}

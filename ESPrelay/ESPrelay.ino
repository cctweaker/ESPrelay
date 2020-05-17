ADC_MODE(ADC_VCC);
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <time.h>
#include <Wire.h>
// #include <ArduinoJson.h>

///////////////////////////////////////////////////////////////////////

#include "private.h"
#include "mqtt.h"
#include "variables.h"

///////////////////////////////////////////////////////////////////////

BearSSL::WiFiClientSecure net;
MQTTClient client(256);
time_t now;

///////////////////////////////////////////////////////////////////////

void setup()
{
  Wire.begin(2, 0);  // connect SDA to GPIO2 and SCL to GPIO0
  setup_MCP();       // set MCP registers
  init_mac_topics(); // get mac and make topics for mqtt
  init_wifi();       // initialize wifi
  ntp_time();        // get ntp time
  certification();   // take care of certs
  mqtt_setup();      // setup & connect mqtt
}

///////////////////////////////////////////////////////////////////////

void loop()
{
  if (!client.connected())
    mqtt_connect();

  client.loop(); // loop MQTT client

  if (heartbeat)
    if ((unsigned long)(millis() - last_heartbeat) > heartbeat_period)
    {
      last_heartbeat = millis();
      send_heartbeat();
    }

  delay(50);
}

///////////////////////////////////////////////////////////////////////
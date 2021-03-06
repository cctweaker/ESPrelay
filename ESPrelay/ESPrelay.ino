ADC_MODE(ADC_VCC);
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <time.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <LittleFS.h>
#include <ESP8266httpUpdate.h>

///////////////////////////////////////////////////////////////////////

// #include "private.h"
#include "sm.h"
// #include "config.h"
#include "mqtt.h"
#include "variables.h"

///////////////////////////////////////////////////////////////////////

BearSSL::WiFiClientSecure net;
MQTTClient client(256);
time_t now;

///////////////////////////////////////////////////////////////////////

void setup()
{
  load_config();     // load configuration from SPIFFS
  init_others();     // other stuff to initialize
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

  /////////////////////////

  if (do_ota_update) // if got command to do OTA update
    ota_update();

  if (do_save_config) // if got command to save config
    save_config();

  if (do_fs_format) // if got command to format FS
    format_fs();

  if (do_list_config) // list config file to serial
    list_config();

  /////////////////////////

  check_timers(); // check timer expiration

  /////////////////////////

  if (heartbeat)
    if ((unsigned long)(millis() - last_heartbeat) > heartbeat_period)
    {
      last_heartbeat = millis();
      send_heartbeat();
    }

  delay(10);
}

///////////////////////////////////////////////////////////////////////

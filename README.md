# ESPlights

MQTT controlling software for the beta version of the 16 channel lights controller provided by https://3dstar.ro

# Features

- secure MQTT connection, Let's Ecrypt CA cetificate already loaded
- topic structure for building control: location/device/+
- lights state saved and recovered on restart from MQTT server
- on / off / toggle commands for each channel
- separate status topic for each channel to show current channel state
- main and backup WiFi connections
- secured, no access point is presented in case both WiFi connections are unavailable
- no web access, just pure MQTT
- easy integration with Home Assistant (not yet automatic)
- extra information on stat topic: type, ID, supply voltage, AP SSID, AP RSSI, AP mac

# To do

- automatic Home Assistant integration
- topic for configuration
- configure new devices by phone/app

# Hardware

Soon to be available from https://3dstar.ro in PCB, kit (PCB + components) or fully assembled form.

The ESP-01 / ESP-01S from AI Thinker is used to control one MCP23017 16bit IO expander. On each channel there are 1 optotriac and 1 triac to control mains powered lights. Each channel can switch up to around 600W without any special cooling but total power draw should be considered.

The controller must be placed after a 10A circuit breaker so total power should not exceed 2300W in Europe. Please conform to your local regulations!
If all channels are used equally, this corresponds to about 137W per channel, enough for 9 x 15W LED bulbs per each channel or a total of 144 15W LED bulbs.
The controller should cover most houses and apartments without problems, leaving a few extra channels for blinds, irrigation or other controls.
The number of independent 16 channels control boards you can use is in theory unlimited but practically the limiting factor will be your electrical installation space and available DHCP IP addresses on WiFi.

# Usage
- download
- rename example_private.h to private.h
- edit private.h
- build & upload (PlatformIO recommended for automatic library management)

- command topic: "location"/lights_abcdef/cmnd/x where abcdef is ESP chip id (last 3 mac address bytes) and x is the channel number to be controlled. Send 1 for ON, 0 for OFF or 2 for TOGGLE. Don't send retained messages here.
- channels status topic: "location"/lights_abcdef/chanx it will be 0 for OFF and 1 for ON. Updated by the ESP after processing command received, statuses are retained.
- status topic: "location"/lights_abcdef/stat where the ESP sends periodic updates with type, ID, supply voltage, SSID, RSSI, BSSID (AP mac)
<code>{"Type":"lights","ID":"abcdef","Vin":3.31,"SSID":"WIFI","RSSI":-29,"BSSID":"D8:50:E6:XX:XX:XX"}</code>
- will topic: "location"/lights_abcdef/will it will be 1 when device is ON and 0 when powered OFF
- save topic: "location"/lights_abcdef/save where the device will save a retained message with its last state for all channels. At startup it will load channel statuses from here.

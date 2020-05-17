# ESPrelay

is a variant of ESPlights. It's a control board for the sainsmart 16 relay board with MQTT communication over WiFi.

# Features

- secure MQTT connection, Let's Ecrypt CA cetificate already loaded
- topic structure for building control: location/device/+
- on / off / toggle commands for each channel
- separate status topic for each channel to show current channel state
- main and backup WiFi connections
- secured, no access point is presented in case both WiFi connections are unavailable
- no web access, just pure MQTT
- extra information on stat topic: type, ID, supply voltage, AP SSID, AP RSSI, AP mac

# To do

- automatic Home Assistant integration
- topic for configuration
- configure new devices by phone/app

# Hardware

soon

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
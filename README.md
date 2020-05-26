# ESPrelay
Variant of ESPlights, it is a controller for the sainsmart 16 relay board with MQTT communication over WiFi.

# Features
- secure MQTT connection, Let's Ecrypt CA cetificate already loaded
- topic structure for multiple buildings (locations) control: <code>location/device_espid/+</code>
- shutter/blinds control with up/stop/down/stop commands and max timeout per shutter (in seconds)
- current shutter status topic based on timeout for end of travel determination
- door control with max timeout per door (in seconds)
- gate control with max timeout per gate (in milliseconds, good for impulses)
- separate status topic for each channel to show current channel state
- main and backup WiFi connections
- secured, no access point is presented in case both WiFi connections are unavailable
- no web access, just pure MQTT
- extra information on stat topic: type, ID, supply voltage, AP SSID, AP RSSI, AP mac


# Hardware
soon

# Build & upload
- there are 2 methods:
    - download, edit config.h, build & upload
    - download, edit config.json, build & upload main program then also upload file system image
- in both cases PlatformIO is recommended for automatic library management

# Usage
- mqtt topic: <code>LOC/TIP_ESPid</code> where LOC & TIP are user defined and ESPid is the HEX representation of your ESP mac address' last 3 bytes. What follows are possible suffixes and must be added after the mqtt topic.
- command topic: <code>/cmnd/...</code>
    - <code>save</code>: save current configuration to LittleFS
    - <code>update</code>: perform OTA update
    - <code>reset</code>: restart ESP device
    - <code>format</code>: format LittleFS space
    - <code>list</code>: list current LittleFS stored config.json to Serial @ 115200bps. Other than this serial is not used at all.
    - <code>config</code>: change device configuration. Due to MQTT message length restrictions do not send json messages longer than 256 bytes. Get examples from config.json in data folder. You can send any part at any time. Remember to send save command after you are satisfied with the configuration.

    - <code>/sX</code>: shutter control. X is the shutter number. Payload can be 0 = stop, 1 = UP, 2 = down. Repeating UP/DOWN also stops the shutters.
    - <code>/dX</code>: door control. X is the door number. Payload can be 0 = off, 1 = on, 2 = toggle
    - <code>/gX</code>: gate control. X is the gate number. Payload can be 0 = off, 1 = on, 2 = toggle
    - <code>/cX</code>: simple/normal control. X is the number. Payload can be 0 = off, 1 = on, 2 = toggle

- channel status topic: <code>/chan/X</code> where X is the channel number. Payload will be 0 for OFF and 1 for ON. It is updated by the ESP after processing commands, statuses are sent with retain flag.
- shutter status topic: <code>/shut/X</code> where X is the shutter number. It holds the current shutter positions. Payload will be 0 for unknown, 1 for UP, 2 for DOWN.
- status topic: <code>/stat</code> where the ESP sends periodic updates (heartbeat) with type, ID, supply voltage, SSID, RSSI, BSSID (AP mac) Example: <code>{"Type":"relays","ID":"abcdef","Vin":3.31,"SSID":"WIFI","RSSI":-29,"BSSID":"D8:50:E6:XX:XX:XX"}</code>
- will topic: <code>/will</code> Payload is 1 when device is ON and 0 when powered OFF
- save topic: <code>/save</code> where the device will save a retained message with its last state for lights. At startup it will load lights statuses from here and start the lights accordingly.

# Libraries
- https://github.com/256dpi/arduino-mqtt
- https://github.com/bblanchon/ArduinoJson

# PCB example
soon
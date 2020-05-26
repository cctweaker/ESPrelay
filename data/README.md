# configuration by LittleFS file

The module can be configured in two ways:

- by editing config.h with default values then normal upload and issue save command by mqtt to save the settings on the ESP. After this save action you can perform OTA updates as the settings are saved on the ESP. The module will work fine with the settings provided in config.h but on OTA update settings will not be saved.

- by editing config.json in data folder and uploading the file system image on the ESP.


<code>
{
    "TIP": "relays",
    "LOC": "location",
    "ssid": "WiFi",
    "pass": "passw0rd",
    "ssidb": "BackupWIFI",
    "passb": "passw0rd",
    "MQTT_HOST": "www.mqtt.server",
    "MQTT_PORT": 8883,
    "MQTT_USER": "mqtt_user",
    "MQTT_PASS": "mqtt_passw0rd",
    "update_url": "https://esp.3dstar.ro/",
    "FACTOR": 1024.0,
    "heartbeat": true,
    "heartbeat_minutes": 10,
    "invert_output": true,
    "relay_to_mcp": [
        8,
        7,
        9,
        6,
        10,
        5,
        11,
        4,
        12,
        3,
        13,
        2,
        14,
        1,
        15,
        0
    ],
    "shutters": 5,
    "shutter_chn_up": [
        1,
        3,
        5,
        7,
        9
    ],
    "shutter_chn_down": [
        2,
        4,
        6,
        8,
        10
    ],
    "shutter_timing": [
        30,
        30,
        30,
        30,
        30
    ],
    "doors": 1,
    "door_chn": [
        15
    ],
    "door_timing": [
        10
    ],
    "gates": 1,
    "gate_chn": [
        16
    ],
    "gate_timing": [
        400
    ],
    "simple": 4,
    "simple_chn": [
        11,
        12,
        13,
        14
    ]
}
</code>

- TIP is the purpose of the device. It will be complemented by _ and 3 bytes read from ESP.getChipId(). It will be used as hostname of the device on your WiFi network. It will also be used in the mqtt topic as name.

- LOC is the location name of the device. It is used as topic prefix.

Topics will look like this: <code>LOC/TIP_ESPid/suffix</code> where ESPid is the hex representation of the last 3 bytes from the ESP mac address.

- SSIDa is your main Wifi
- PASSa is your main Wifi password
- SSIDb is your secondary Wifi (can be an Android hotspot)
- PASSb is your secondary Wifi password

- MQTT_HOST is the name of your mqtt server (can hold IP but then the SSL cert will fail)
- MQTT_PORT is your secure mqtt port
- MQTT_USER
- MQTT_PASS

- update_url: is your update URL. You can leave the default one as updates will be pushed to that location when released. Updates are not automatic, if your device works fine then there is no need to update unless there was a critical safety feature update or a you need a new feature that was added. The url will be appended by TIP and VERSION to get the correct file for updating. Always end the default URL with <code>/</code> or <code>.php</code>

- FACTOR: is the correction applied to get a real ESP supply voltage readout. To calculate it use the following formula: <code>FACTOR = 1024.0 * MEASURED_VOLTAGE / REPORTED_VOLTAGE</code> aka <code>FACTOR = 1024.0 * 3.294 / 3.43</code>

- heartbeat: true or false. Pick true if you want a periodic report over mqtt. It will contain useful info about your module.

- heartbeat_minutes is the time in minutes between heartbeat messages. The default is 10 minutes.

- relay_to_mcp: convert relay numbers (1-16) to MCP23017 pins (PortB:PortA MSB->LSB)

- shutters: define the number of shutters you want to control (max 8 because each shutter uses 2 channels)
- shutter_chn_up: channel connected to up line of shutter, for each shutter from 1 to max.
- shutter_chn_down: channel connected to down line of shutter, for each shutter from 1 to max.
- shutter_timing: set the max timeout in seconds for each shutter from 1 to max. When this time passes the shutter is reported as fully closed/open. If you stop its operation then shutter status will be 0 (undefined).

- doors: define the number of door controls to manage (max 16)
- door_chn: channel connected to door control line, for each door from 1 to max.
- door_timing: set the timeout per door, in seconds, for each door from 1 to max. 0-256s.

- gates: define the number of gate controls to manage (max 16)
- gate_chn: channel connected to gate control line, for each gate from 1 to max.
- gate_timing: set the timeout per gate, in milliseconds, for each gate from 1 to max. 0-65535ms

- simple: define the number of simple/normal controls to manage (max 16)
- simple_chn: channel connected to normal control line, from 1 to max.
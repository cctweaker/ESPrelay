//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void load_config()
{
    if (!LittleFS.begin())
        return;

    // open config file for reading
    File configFile = LittleFS.open(config_file, "r");
    if (!configFile)
    {
        return;
    }

    uint8_t i = 0;
    StaticJsonDocument<2048> doc;

    // Read content from config file.
    deserializeJson(doc, configFile);
    configFile.close();
    LittleFS.end();

    if (doc.containsKey("TIP"))
        strlcpy(TIP, doc["TIP"], sizeof(TIP));
    if (doc.containsKey("LOC"))
        strlcpy(LOC, doc["LOC"], sizeof(LOC));

    if (doc.containsKey("SSIDa"))
        strlcpy(SSIDa, doc["SSIDa"], sizeof(SSIDa));
    if (doc.containsKey("PASSa"))
        strlcpy(PASSa, doc["PASSa"], sizeof(PASSa));
    if (doc.containsKey("SSIDb"))
        strlcpy(SSIDb, doc["SSIDb"], sizeof(SSIDb));
    if (doc.containsKey("PASSb"))
        strlcpy(PASSb, doc["PASSb"], sizeof(PASSb));

    if (doc.containsKey("MQTT_HOST"))
        strlcpy(MQTT_HOST, doc["MQTT_HOST"], sizeof(MQTT_HOST));
    if (doc.containsKey("MQTT_PORT"))
        MQTT_PORT = doc["MQTT_PORT"];
    if (doc.containsKey("MQTT_USER"))
        strlcpy(MQTT_USER, doc["MQTT_USER"], sizeof(MQTT_USER));
    if (doc.containsKey("MQTT_PASS"))
        strlcpy(MQTT_PASS, doc["MQTT_PASS"], sizeof(MQTT_PASS));

    if (doc.containsKey("update_url"))
        strlcpy(update_url, doc["update_url"], sizeof(update_url));

    if (doc.containsKey("FACTOR"))
        FACTOR = doc["FACTOR"];
    if (doc.containsKey("heartbeat"))
        heartbeat = doc["heartbeat"];
    if (doc.containsKey("heartbeat_minutes"))
        heartbeat_minutes = doc["heartbeat_minutes"];

    if (doc.containsKey("invert_output"))
        invert_output = doc["invert_output"];

    if (doc.containsKey("relay_to_mcp"))
        for (i = 0; i < 16; i++)
            relay_to_mcp[i] = doc["relay_to_mcp"][i];

    if (doc.containsKey("shutters"))
        shutters = doc["shutters"];
    for (i = 0; i < shutters; i++)
    {
        if (doc.containsKey("shutter_chn_up"))
            shutter_chn_up[i] = doc["shutter_chn_up"][i];
        if (doc.containsKey("shutter_chn_down"))
            shutter_chn_down[i] = doc["shutter_chn_down"][i];
        if (doc.containsKey("shutter_timing"))
            shutter_timing[i] = doc["shutter_timing"][i];
    }

    if (doc.containsKey("doors"))
        doors = doc["doors"];
    for (i = 0; i < doors; i++)
    {
        if (doc.containsKey("door_chn"))
            door_chn[i] = doc["door_chn"][i];
        if (doc.containsKey("door_timing"))
            door_timing[i] = doc["door_timing"][i];
    }

    if (doc.containsKey("gates"))
        gates = doc["gates"];
    for (i = 0; i < gates; i++)
    {
        if (doc.containsKey("gate_chn"))
            gate_chn[i] = doc["gate_chn"][i];
        if (doc.containsKey("gate_timing"))
            gate_timing[i] = doc["gate_timing"][i];
    }

    if (doc.containsKey("simple"))
        simple = doc["simple"];
    for (i = 0; i < simple; i++)
        if (doc.containsKey("simple_chn"))
            simple_chn[i] = doc["simple_chn"][i];

    doc.clear();
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void save_config()
{
    uint8_t i = 0;

    do_save_config = false;

    if (!LittleFS.begin())
        return;

    StaticJsonDocument<2048> doc;

    doc["TIP"] = TIP;
    doc["LOC"] = LOC;

    doc["SSIDa"] = SSIDa;
    doc["PASSa"] = PASSa;
    doc["SSIDb"] = SSIDb;
    doc["PASSb"] = PASSb;

    doc["MQTT_HOST"] = MQTT_HOST;
    doc["MQTT_PORT"] = MQTT_PORT;
    doc["MQTT_USER"] = MQTT_USER;
    doc["MQTT_PASS"] = MQTT_PASS;

    doc["update_url"] = update_url;

    doc["FACTOR"] = FACTOR;
    doc["heartbeat"] = heartbeat;
    doc["heartbeat_minutes"] = heartbeat_minutes;

    doc["invert_output"] = invert_output;

    for (i = 0; i < 16; i++)
        doc["relay_to_mcp"].add(relay_to_mcp[i]);

    doc["shutters"] = shutters;
    for (i = 0; i < shutters; i++)
    {
        doc["shutter_chn_up"].add(shutter_chn_up[i]);
        doc["shutter_chn_down"].add(shutter_chn_down[i]);
        doc["shutter_timing"].add(shutter_timing[i]);
    }

    doc["doors"] = doors;
    for (i = 0; i < doors; i++)
    {
        doc["door_chn"].add(door_chn[i]);
        doc["door_timing"].add(door_timing[i]);
    }

    doc["gates"] = gates;
    for (i = 0; i < gates; i++)
    {
        doc["gate_chn"].add(gate_chn[i]);
        doc["gate_timing"].add(gate_timing[i]);
    }

    doc["simple"] = simple;
    for (i = 0; i < simple; i++)
        doc["simple_chn"].add(simple_chn[i]);

    // serialize & write config
    File configFile = LittleFS.open(config_file, "w");
    if (!configFile)
        return;

    serializeJson(doc, configFile);
    configFile.close();

    doc.clear();

    LittleFS.end();

    client.publish(MQTT_STAT_TOPIC, "Save complete.", false, 0);
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void format_fs()
{
    do_fs_format = false;

    if (!LittleFS.begin())
        return;

    LittleFS.format();

    LittleFS.end();

    client.publish(MQTT_STAT_TOPIC, "Format complete.", false, 0);
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void list_config()
{
    do_list_config = false;

    if (!LittleFS.begin())
        return;

    // open config file for reading
    File configFile = LittleFS.open(config_file, "r");

    Serial.begin(115200);
    Serial.println();
    Serial.println(&config_file[0]);

    while (configFile.available())
        Serial.print((char)configFile.read());

    LittleFS.end();
    delay(500);
    Serial.end();
}
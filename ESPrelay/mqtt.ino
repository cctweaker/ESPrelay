//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void mqtt_setup()
{
    client.begin(MQTT_HOST, MQTT_PORT, net);
    client.setWill(MQTT_WILL_TOPIC, "0", true, 0);
    client.onMessage(messageReceived);
    mqtt_connect();
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void mqtt_connect()
{
    uint8_t i = 0;

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(100);
        i++;
        if (i > 100)
            ESP.restart();
    }

    // certificare
    BearSSL::X509List cert(digicert);
    net.setTrustAnchors(&cert);

    while (!client.connect(HOSTNAME, MQTT_USER, MQTT_PASS))
    {
        delay(100);
        i++;
        if (i > 100)
            ESP.restart();
    }

    client.subscribe(MQTT_CMND_TOPIC, 0);
    client.publish(MQTT_WILL_TOPIC, "1", true, 0);
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void messageReceived(String &topic, String &payload)
{

    uint8_t idx = topic.lastIndexOf('/') + 1;
    String cmnd = topic.substring(idx);

    if (cmnd == "save")
    {
        do_save_config = true;
        return;
    }

    if (cmnd == "update")
    {
        do_ota_update = true;
        return;
    }

    if (cmnd == "reset")
    {
        ESP.restart();
    }

    if (cmnd == "format")
    {
        do_fs_format = true;
        return;
    }

    if (cmnd == "list")
    {
        do_list_config = true;
        return;
    }

    /////////////////

    if (cmnd == "config")
    {
        uint8_t i = 0;

        StaticJsonDocument<1024> doc;
        deserializeJson(doc, payload);

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
        if (doc.containsKey("shutter_chn_up"))
            for (i = 0; i < shutters; i++)
                shutter_chn_up[i] = doc["shutter_chn_up"][i];
        if (doc.containsKey("shutter_chn_down"))
            for (i = 0; i < shutters; i++)
                shutter_chn_down[i] = doc["shutter_chn_down"][i];
        if (doc.containsKey("shutter_timing"))
            for (i = 0; i < shutters; i++)
                shutter_timing[i] = doc["shutter_timing"][i];

        if (doc.containsKey("doors"))
            doors = doc["doors"];
        if (doc.containsKey("door_chn"))
            for (i = 0; i < doors; i++)
                door_chn[i] = doc["door_chn"][i];
        if (doc.containsKey("door_timing"))
            for (i = 0; i < doors; i++)
                door_timing[i] = doc["door_timing"][i];

        if (doc.containsKey("gates"))
            gates = doc["gates"];
        if (doc.containsKey("gate_chn"))
            for (i = 0; i < gates; i++)
                gate_chn[i] = doc["gate_chn"][i];
        if (doc.containsKey("gate_timing"))
            for (i = 0; i < gates; i++)
                gate_timing[i] = doc["gate_timing"][i];

        if (doc.containsKey("simple"))
            simple = doc["simple"];
        if (doc.containsKey("simple_pin"))
            for (i = 0; i < simple; i++)
                simple_chn[i] = doc["simple_chn"][i];

        doc.clear();
        init_others();
        return;
    }

    /////////////////

    cmnd = topic.substring(idx, idx + 1);
    String cmndid = topic.substring(idx + 1);
    uint8_t id = cmndid.toInt() - 1;
    uint8_t value = payload.toInt();

    if (cmnd == "s") // shutters
    {
        client.publish(MQTT_SHUT_TOPIC + String(id + 1), "0", true, 0);

        if (shutter_status[id] == 0)
        {
            if (value == 1)
            {
                shutter_status[id] = 1;
                update_pins(shutter_chn_up[id], 1);
                shutter_timeout[id] = millis();
            }
            if (value == 2)
            {
                shutter_status[id] = 2;
                update_pins(shutter_chn_down[id], 1);
                shutter_timeout[id] = millis();
            }
        }
        else
        {
            update_pins(shutter_chn_up[id], 0);
            update_pins(shutter_chn_down[id], 0);
            shutter_status[id] = 0;
            shutter_timeout[id] = 0;
        }
    }

    if (cmnd == "d") // doors
    {
        door_timeout[id] = millis();
        update_pins(door_chn[id], 1);
    }

    if (cmnd == "g") // gates
    {
        gate_timeout[id] = millis();
        update_pins(gate_chn[id], 1);
    }

    if (cmnd == "c") // simple channels
    {
        update_pins(simple_chn[id], value);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void init_mac_topics()
{
    sprintf(HOSTNAME, "%s_%x", TIP, ESP.getChipId());
    sprintf(MQTT_WILL_TOPIC, "%s/%s_%x%s", LOC, TIP, ESP.getChipId(), WILL);
    sprintf(MQTT_CHAN_TOPIC, "%s/%s_%x%s", LOC, TIP, ESP.getChipId(), CHAN);
    sprintf(MQTT_CMND_TOPIC, "%s/%s_%x%s", LOC, TIP, ESP.getChipId(), CMND);
    sprintf(MQTT_STAT_TOPIC, "%s/%s_%x%s", LOC, TIP, ESP.getChipId(), STAT);
    sprintf(MQTT_SHUT_TOPIC, "%s/%s_%x%s", LOC, TIP, ESP.getChipId(), SHUT);
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void send_heartbeat()
{
    char tx[256];
    float supply = (float)ESP.getVcc() / FACTOR;

    sprintf(tx, "{\"Type\":\"%s\",\"v\":%d,\"ID\":\"%x\",\"Vin\":%.2f,\"SSID\":\"%s\",\"RSSI\":%d,\"BSSID\":\"%s\"}", TIP, VERSION, ESP.getChipId(), supply, WiFi.SSID().c_str(), WiFi.RSSI(), WiFi.BSSIDstr().c_str());

    client.publish(MQTT_STAT_TOPIC, tx, true, 0);
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

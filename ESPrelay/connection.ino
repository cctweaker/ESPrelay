//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void init_wifi()
{
    uint8_t i = 0;

    // initializare WIFI
    WiFi.hostname(HOSTNAME);
    WiFi.mode(WIFI_STA);

    WiFi.begin(SSIDa, PASSa);

    i = 0;
    while (WiFi.status() != WL_CONNECTED && i < 100)
    {
        delay(100);
        i++;
    }

    // if first network not reachable try second one
    if (WiFi.status() != WL_CONNECTED)
    {
        WiFi.begin(SSIDb, PASSb);

        i = 0;
        while (WiFi.status() != WL_CONNECTED)
        {
            delay(100);
            i++;
            if (i > 200)
                ESP.restart();
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void ntp_time()
{
    // NTP time
    configTime(+3 * 3600, 0, "esp.3dstar.ro", "3dstar.ro", "pool.ntp.org");
    now = time(nullptr);
    while (now < 1510592825)
    {
        delay(200);
        now = time(nullptr);
    }
    struct tm timeinfo;
    gmtime_r(&now, &timeinfo);
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void certification()
{
    BearSSL::X509List cert(digicert);
    net.setTrustAnchors(&cert);
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void ota_update()
{
    do_ota_update = false;

    // disconnect MQTT
    client.disconnect();

    BearSSL::X509List cert(digicert);
    net.setTrustAnchors(&cert);

    // make update_url complete
    char url[128];
    sprintf(url, "%s?t=%s&v=%d", update_url, TIP, VERSION);

    // check 4 updates
    ESPhttpUpdate.update(net, url);

    ESP.restart();
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void init_others()
{
  heartbeat_period = heartbeat_minutes * 60 * 1000;
}
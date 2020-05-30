//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void check_MCP()
{
    Wire.beginTransmission(MCP_ADDR);

    if (Wire.endTransmission() == 0)
    {
        // MCP found!
        MCP = true;
        return;
    }

    MCP = false;
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void setup_MCP()
{
    uint8_t byte = 0x00;

    // set all to low
    Wire.beginTransmission(MCP_ADDR);
    Wire.write(GPIOA); // GPIOA
    Wire.write(byte);  // GPIOA
    Wire.write(byte);  // GPIOB
    Wire.write(byte);  // OLATA
    Wire.write(byte);  // OLATB
    Wire.endTransmission();

    if (invert_output)
    {
        byte = 0xFF;   // set all inputs otherwise outputs
        pins = 0xFFFF; // initialize all pins as high (relay active low)
    }

    Wire.beginTransmission(MCP_ADDR);
    Wire.write(0x00); // IODIRA address
    Wire.write(byte); // IODIRA
    Wire.write(byte); // IODIRB
    Wire.endTransmission();
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void update_pins(uint8_t pin, uint8_t value, bool show_val = false)
{
    if (pin == 0 || pin > 16)
        return;

    uint8_t pos = relay_to_mcp[pin - 1];

    uint8_t val = 0;

    if (value > 1 && !show_val)
        val = !bitRead(pins, pos);

    if (value > 1 && show_val)
        if (!invert_output)
            val = 1;

    if (invert_output)
    {
        if (value == 0)
            val = 1;
    }
    else
    {
        if (value == 1)
            val = 1;
    }

    bitWrite(pins, pos, val);

    update_MCP();

    client.publish(MQTT_CHAN_TOPIC + String(pin), String(show_val ? value : invert_output ? !val : val), true, 0);
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void update_MCP()
{
    uint8_t portA = pins;
    uint8_t portB = pins >> 8;

    if (invert_output) // also change direction if inverted
    {
        Wire.beginTransmission(MCP_ADDR);
        Wire.write(0x00); // IODIR
        Wire.write(portA);
        Wire.write(portB);
        Wire.endTransmission();

        Wire.beginTransmission(MCP_ADDR);
        Wire.write(GPIOA); // GPIOA
        Wire.write(0x00);  // GPIOA
        Wire.write(0x00);  // GPIOB
        Wire.endTransmission();
    }
    else
    {
        Wire.beginTransmission(MCP_ADDR);
        Wire.write(GPIOA);
        Wire.write(portA);
        Wire.write(portB);
        Wire.endTransmission();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

void check_timers()
{
    uint8_t i = 0;

    for (i = 0; i < shutters; i++) // test all shutter timeouts
    {
        if (shutter_timeout[i] > 0)
        {
            if ((unsigned long)(millis() - shutter_timeout[i]) > shutter_timing[i] * 1000)
            {
                client.publish(MQTT_SHUT_TOPIC + String(i + 1), String(shutter_status[i]), true, 0);
                update_pins(shutter_chn_up[i], 0);
                update_pins(shutter_chn_down[i], 0);
                shutter_timeout[i] = 0;
                shutter_status[i] = 0;
            }
        }
    }

    /////////////////////////

    for (i = 0; i < doors; i++) // test all door timeouts
    {
        if (door_timeout[i] > 0)
        {
            if ((unsigned long)(millis() - door_timeout[i]) > door_timing[i] * 1000)
            {
                update_pins(door_chn[i], 0);
                door_timeout[i] = 0;
            }
        }
    }

    /////////////////////////

    for (i = 0; i < gates; i++) // test all gate timeouts
    {
        if (gate_timeout[i] > 0)
        {
            if ((unsigned long)(millis() - gate_timeout[i]) > gate_timing[i])
            {
                update_pins(gate_chn[i], 0);
                gate_timeout[i] = 0;
            }
        }
    }
}
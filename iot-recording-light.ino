TCPServer server = TCPServer(80);
TCPClient client;

enum LED_PINS {
    RED_LED   = D1,
    GREEN_LED = D2,
    BLUE_LED  = D0
};

enum LED_COLORS {
    WHITE  = 7,
    YELLOW = 6,
    RED    = 4,
    GREEN  = 2,
    BLUE   = 1,
    BLACK  = 0
};


void leds_color(LED_COLORS c)
{
    digitalWrite(RED_LED,   c & 0b100);
    digitalWrite(GREEN_LED, c & 0b010);
    digitalWrite(BLUE_LED,  c & 0b001);
}

void leds_off()
{
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(BLUE_LED, LOW);
}

/* Blink the color c for t milliseconds
 */
void blink(LED_COLORS c, int t)
{
    leds_color(c);
    delay(t);
    leds_off();
}


// Initialization code
void setup()
{
    server.begin();
    pinMode(RED_LED, OUTPUT);
    pinMode(GREEN_LED, OUTPUT);
    pinMode(BLUE_LED, OUTPUT);
    blink(YELLOW, 250);
    delay(1000);
    for (int i = 0; i < 256; i++)
    {
        analogWrite(RED_LED, i);
        delay(5);
    }
}

enum RECORDING_STATE {
    OFF,
    ARMED,
    RECORDING
};

enum RECORDING_STATE current_state = OFF;

// Loop code
long off_timer = millis();
bool off_timer_stat = false;
int off_thresh = 2000;

long on_timer = millis();
bool on_timer_stat = true;
int on_thresh  = 750;

long display_timer = micros();
int display_period = 200;

int cur_color_vals[] = {0, 0, 0};
int color_vals[] = {0, 0, 0};

void loop()
{
    
    if ((micros() - display_timer) > display_period)
    {
        display_timer = micros();
        for (int i = 0; i < 3; i++)
        {
            if (cur_color_vals[i] < color_vals[i])
            {
                cur_color_vals[i]++;
            }
            else if (cur_color_vals[i] > color_vals[i])
            {
                cur_color_vals[i]--;
            }
        }
        analogWrite(RED_LED, cur_color_vals[0]);
        analogWrite(GREEN_LED, cur_color_vals[1]);
        analogWrite(BLUE_LED, cur_color_vals[2]);
    }
    
    if (current_state == OFF)
    {
        color_vals[0] = 0;
        color_vals[1] = 0;
        color_vals[2] = 0;
    }
    else if (current_state == ARMED)
    {
        color_vals[0] = 255;
        color_vals[1] = 150;
        color_vals[2] = 130;
    }
    else if (current_state == RECORDING)
    {
        color_vals[0] = 255;
        color_vals[1] = 0;
        color_vals[2] = 0;
    }
    
    if (off_timer_stat)
    {
        if ((millis() - off_timer) > off_thresh)
        {
            current_state = OFF;
            off_timer_stat = false;
        }
    }
    
    // recieve the TCP connections
    if (client.connected())
    {
        
        // gather the data bytes
        uint8_t data;
        while (client.available())
        {
            data = client.read();
        }
        
        if (data == 0x7F)
        {
            current_state = RECORDING;
            on_timer = millis();
            off_timer_stat = false;
        }
        else if (data == 0x7E)
        {
            if (current_state == RECORDING)
            {
                // only switch if on_thresh milliseconds have ellapsed since
                // recording began
                if ((millis() - on_timer) > on_thresh)
                {
                    current_state = ARMED;
                    off_timer_stat = false;
                }
                // otherwise it was the corner case
            }
            else
            {
                current_state = ARMED;
                off_timer_stat = false;
            }
        }
        else if (data == 0x01)
        {
            off_timer_stat = true;
            off_timer = millis();
        }
        else if (data == 0x0)
        {
            // this is pointless
        }
    }
    else
    {
        client = server.available();
    }
    
}
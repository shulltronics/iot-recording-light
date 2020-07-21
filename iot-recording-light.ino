#include <WebServer.h>

/* This creates an instance of the webserver.  By specifying a prefix
 * of "", all pages will be at the root of the server. */
#define PREFIX ""
WebServer webserver(PREFIX, 80);
// the HTML code for the webpage
P(html1) = "<html> <head> <title>Recording Light</title> </head> <body style=\"background-image:linear-gradient(black,#5C0E0E);color:gray;font-family:'courier-new',monospace;\">   <div style=\"text-align:center\"> <div style=\"display:inline-block;padding:10pt 70pt;margin:70pt 0;border:solid 3px #ECF0F1;border-radius:10px\"> <div style=\"font-size:20pt;padding:10pt 0;\">Recording Light Control</div> <form id=\"on-style-form\" method=\"post\"> <p style=\"margin:10pt;\"><u>Light Transition Style</u></p> <div> <input type=\"radio\" name=\"on-style\" id=\"on-style\" value=\"1\" ";
P(html2) = "> <label for=\"fade-in\">Fade in</label> </div> <div> <input type=\"radio\" name=\"on-style\" id=\"on-style\" value=\"0\" ";
P(html3) = "> <label for=\"immediate\">Immediate</label> </div> <p style=\"margin:5pt;\"><u>Armed Color</u></p> <div> <label for=\"red\">Red: </label> <input type=\"range\" name=\"armed-red\" id=\"armed-color\" min=\"0\" max=\"255\" step=\"1\" value=";
P(html4) = "> </div> <div> <label for=\"green\">Green: </label> <input type=\"range\" name=\"armed-green\" id=\"armed-color\" min=\"0\" max=\"255\" step=\"1\" value=";
P(html5) = "> </div> <div> <label for=\"blue\">Blue: </label> <input type=\"range\" name=\"armed-blue\" id=\"armed-color\" min=\"0\" max=\"255\" step=\"1\" value=";
P(html6) = "> </div> <p style=\"margin:5pt;\"><u>Recording Color</u></p> <div> <label for=\"red\">Red: </label> <input type=\"range\" name=\"recording-red\" id=\"armed-color\" min=\"0\" max=\"255\" step=\"1\" value=";
P(html7) = "> </div> <div> <label for=\"green\">Green: </label> <input type=\"range\" name=\"recording-green\" id=\"armed-color\" min=\"0\" max=\"255\" step=\"1\" value=";
P(html8) = "> </div> <div> <label for=\"blue\">Blue: </label> <input type=\"range\" name=\"recording-blue\" id=\"armed-color\" min=\"0\" max=\"255\" step=\"1\" value=";
P(html9) = "> </div> <div style=\"margin:10pt;\"> <button type=\"submit\">Submit</button> </div> </form> </div> </div> </body> </html>";
//TCPServer server = TCPServer(80);
//TCPClient client;

#define TRANS_TYPE_EEPROM_ADDR 0
#define ARMED_COLORS_EEPROM_ADDR 1
#define RECORDING_COLORS_EEPROM_ADDR 4

enum LED_PINS {
    RED_LED   = D1,
    GREEN_LED = D2,
    BLUE_LED  = D0
};

enum TRANSITION_TYPES {
    IMMEDIATE = 0,
    FADE      = 1
};

enum TRANSITION_TYPES trans_type = IMMEDIATE;

int armed_color_vals[3];
int recording_color_vals[3];

void leds_off()
{
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(BLUE_LED, LOW);
}

/* Blink the RGB color for t milliseconds
 */
void blink(int red, int grn, int blu, int t)
{
    analogWrite(RED_LED, red);
    analogWrite(GREEN_LED, grn);
    analogWrite(BLUE_LED, blu);
    delay(t);
    leds_off();
}

enum RECORDING_STATE {
    OFF,
    ARMED,
    RECORDING
};

enum RECORDING_STATE current_state = OFF;

int cur_color_vals[] = {0, 0, 0};
int color_vals[] = {0, 0, 0};

// update the state of the LED to the values in global variable cur_color_vals
void leds_update()
{
    analogWrite(RED_LED, cur_color_vals[0]);
    analogWrite(GREEN_LED, cur_color_vals[1]);
    analogWrite(BLUE_LED, cur_color_vals[2]);
}

// print the web interface with current system parameters
void printHTML()
{
    webserver.printP(html1);
    if (trans_type == 1)
    {
        webserver.print("Checked");
    }
    webserver.printP(html2);
    if (trans_type == 0)
    {
        webserver.print("Checked");
    }
    webserver.printP(html3);
    webserver.print(armed_color_vals[0]);
    webserver.printP(html4);
    webserver.print(armed_color_vals[1]);
    webserver.printP(html5);
    webserver.print(armed_color_vals[2]);
    webserver.printP(html6);
    webserver.print(recording_color_vals[0]);
    webserver.printP(html7);
    webserver.print(recording_color_vals[1]);
    webserver.printP(html8);
    webserver.print(recording_color_vals[2]);
    webserver.printP(html9);
}

void ctlCmd(WebServer &server, WebServer::ConnectionType type, char *, bool)
{
    /* this line sends the standard "we're all OK" headers back to the
     browser */
    webserver.httpSuccess();

    if (type == WebServer::POST)
    {
        // gather the POST variables and update our variables here
        char name[16], value[16];
        while (webserver.readPOSTparam(name, 16, value, 16))
        {
            // process the transition types
            if (strcmp(name, "on-style") == 0)
            {
                trans_type = (TRANSITION_TYPES)strtoul(value, NULL, 10);
            }
            // process the track-armed colors
            else if (strcmp(name, "armed-red") == 0)
            {
                armed_color_vals[0] = strtoul(value, NULL, 10);
            }
            else if (strcmp(name, "armed-green") == 0)
            {
                armed_color_vals[1] = strtoul(value, NULL, 10);
            }
            else if (strcmp(name, "armed-blue") == 0)
            {
                armed_color_vals[2] = strtoul(value, NULL, 10);
            }
            // process the track-recording color
            else if (strcmp(name, "recording-red") == 0)
            {
                recording_color_vals[0] = strtoul(value, NULL, 10);
            }
            else if (strcmp(name, "recording-green") == 0)
            {
                recording_color_vals[1] = strtoul(value, NULL, 10);
            }
            else if (strcmp(name, "recording-blue") == 0)
            {
                recording_color_vals[2] = strtoul(value, NULL, 10);
            }
        }
        
        // save the new system parameters to EEPROM
        EEPROM.put(TRANS_TYPE_EEPROM_ADDR, trans_type);
        for (int i = 0; i < 3; i++)
        {
            EEPROM.put(ARMED_COLORS_EEPROM_ADDR + i, (uint8_t)armed_color_vals[i]);
        }
        for (int i = 0; i < 3; i++)
        {
            EEPROM.put(RECORDING_COLORS_EEPROM_ADDR + i, (uint8_t)recording_color_vals[i]);
        }
    
        // give the user some feedback
        blink(armed_color_vals[0], armed_color_vals[1], armed_color_vals[2], 500);
        blink(recording_color_vals[0], recording_color_vals[1], recording_color_vals[2], 500);
        printHTML();
        
    }
    
    else if (type == WebServer::GET)
    {
        printHTML();
    }
    
}

// Timer variables
long off_timer = millis();
bool off_timer_stat = false;
int off_thresh = 2000;

long on_timer = millis();
bool on_timer_stat = true;
int on_thresh  = 750;

long display_timer = micros();
int display_period = 200;


void offCmd(WebServer &server, WebServer::ConnectionType type, char *, bool)
{
    webserver.httpSuccess();
    off_timer_stat = true;
    off_timer = millis();
}

void armCmd(WebServer &server, WebServer::ConnectionType type, char *, bool)
{
    webserver.httpSuccess();
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

void recCmd(WebServer &server, WebServer::ConnectionType type, char *, bool)
{
    webserver.httpSuccess();
    current_state = RECORDING;
    on_timer = millis();
    off_timer_stat = false;
}


// Initialization code
void setup()
{
    pinMode(RED_LED, OUTPUT);
    pinMode(GREEN_LED, OUTPUT);
    pinMode(BLUE_LED, OUTPUT);
    
    // populate the system variables from EEPROM
    uint8_t t;
    EEPROM.get(TRANS_TYPE_EEPROM_ADDR, t);
    if (t == 0xFF)
    {
        trans_type = IMMEDIATE;
    }
    else
    {
        trans_type = (TRANSITION_TYPES)t;
    }
    
    for (int i = 0; i < 3; i++)
    {
        uint8_t val;
        EEPROM.get(ARMED_COLORS_EEPROM_ADDR + i, val);
        if (val == 0xFF)
        {
            armed_color_vals[i] = 255;
        }
        else
        {
            armed_color_vals[i] = val;
        }
    }
    
    for (int i = 0; i < 3; i++)
    {
        uint8_t val;
        EEPROM.get(RECORDING_COLORS_EEPROM_ADDR + i, val);
        if (val == 0xFF)
        {
            recording_color_vals[i] = 255;
        }
        else
        {
            recording_color_vals[i] = val;
        }
    }
    
    blink(armed_color_vals[0], armed_color_vals[1], armed_color_vals[2], 500);
    blink(recording_color_vals[0], recording_color_vals[1], recording_color_vals[2], 500);
    
    webserver.setDefaultCommand(&ctlCmd);
    webserver.addCommand("off.html", &offCmd);
    webserver.addCommand("arm.html", &armCmd);
    webserver.addCommand("rec.html", &recCmd);
    webserver.begin();
    
}


void loop()
{
    
    // Set the color depending on the current state
    if (current_state == OFF)
    {
        color_vals[0] = 0;
        color_vals[1] = 0;
        color_vals[2] = 0;
    }
    else if (current_state == ARMED)
    {
        color_vals[0] = armed_color_vals[0];
        color_vals[1] = armed_color_vals[1];
        color_vals[2] = armed_color_vals[2];
    }
    else if (current_state == RECORDING)
    {
        color_vals[0] = recording_color_vals[0];
        color_vals[1] = recording_color_vals[1];
        color_vals[2] = recording_color_vals[2];
    }
    
    if (off_timer_stat)
    {
        if ((millis() - off_timer) > off_thresh)
        {
            current_state = OFF;
            off_timer_stat = false;
        }
    }
    
    char buff[64];
    int len = 64;
    webserver.processConnection(buff, &len);
    
    if (trans_type == IMMEDIATE)
    {
        for (int i = 0; i < 3; i++)
        {
            cur_color_vals[i] = color_vals[i];
        }
    }
    else if (trans_type == FADE)
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
        }
    }
    
    leds_update();
    
}

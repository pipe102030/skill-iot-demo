#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

// add the neopixel ring to our ESP8266/Huzzah feather project.
#include <Adafruit_NeoPixel.h>


#include <Stepper.h>

#define NEOPIXELPIN 12
#define NUMPIXELS 12
#define COLORINTERVAL 3000
#define SERVOAINTERVAL 15

#define STEPS 25

uint32_t black, white, blue, green, orange, red, red_orange, sky_blue, purple, yellow, pink;
uint32_t standardColors[11];
int currentColor;
long currentTime, previousTime;
boolean colorChange;
int colorIndex;

const char* ssid = "";
const char* password = "";

ESP8266WebServer server(80);
Adafruit_NeoPixel neoRing = Adafruit_NeoPixel(NUMPIXELS, NEOPIXELPIN, NEO_RGBW + NEO_KHZ800);
Stepper stepper(STEPS, 4, 5, 2, 14);

// web server hanlder functions

void handleRoot() 
{ 
  server.send(200, "text/plain", "hello from esp8266!");
}

void handleCommand()
{
  String message = "done";
  int commandValue;
  if (server.arg("cmd") == "")
  {
    message = "no parameter value received";
  }
  else 
  {
    commandValue = (server.arg("cmd")).toInt();
    switch(commandValue)
    {
      case 0:
        currentColor = 0;
        setLampColor();
        break;
      case 1:
        currentColor = 1;
        setLampColor();
        break;
      default:
        message = "choose from 0, 1 for lamp off/on";
        break;     
    }  
  }
   // server.send(200, "text/plain", "this works as well");
   server.send(200, "text/plain", message);
}  

void handleColor()
{
  String message = "done";
  int commandValue;
  if (server.arg("color") == "")
  {
    message = "no parameter value received";
  }
  else 
  {
    currentColor = (server.arg("color")).toInt();
    Serial.print("Color index selected: ");Serial.println(currentColor);
    setLampColor();
  }
   // server.send(200, "text/plain", "this works as well");
  server.send(200, "text/plain", message);
}

void handleStepper()
{
  stepperSweep();
  server.send(200, "text/plain", "servo");
  Serial.println("stepper");
}


void setup(void)
{

  setStandardColors();
  colorChange = false;
  colorIndex = 0;
  previousTime = 0;
  currentTime = 0;
  currentColor = 1; // the default color is white
  neoRing.begin();
  neoRing.show();

  stepper.setSpeed(60);
  
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  // we will need the IP address to send http comands to out board.
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/lamp", handleCommand);
  server.on("/color", handleColor);
  server.on("/stepper",handleStepper);

  server.begin();
  Serial.println("HTTP server started");
}

void setStandardColors()
{
  black   = neoRing.Color(0,0,0); //used for 'off' state
  white   = neoRing.Color(255,255,255);
  blue    = neoRing.Color(0,0,255);
  green   = neoRing.Color(255,0,0);
  orange  = neoRing.Color(140,255,0);
  red     = neoRing.Color(0,255,0);
  red_orange  = neoRing.Color(69,255,0);
  sky_blue    = neoRing.Color(206,135,235);
  purple      = neoRing.Color(0,255,255);
  yellow      = neoRing.Color(255,255,0);
  pink        = neoRing.Color(0,255,180);
 
  standardColors[0] = black;
  standardColors[1] = white;
  standardColors[2] = blue;
  standardColors[3] = green;
  standardColors[4] = orange;
  standardColors[5] = red;
  standardColors[6] = red_orange;
  standardColors[7] = sky_blue;
  standardColors[8] = purple;
  standardColors[9] = yellow;
  standardColors[10] = pink;
}

void setLampColor()
{
  for(int i=0;i<NUMPIXELS;i++)
  {
    neoRing.setPixelColor(i, standardColors[currentColor]);  
  }
  neoRing.show();
}

void stepperSweep()
{
  stepper.step(-STEPS);
  delay(100);
  stepper.step(STEPS);
}

void loop(void)
{
  server.handleClient();

}

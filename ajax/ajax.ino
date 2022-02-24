#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

const char* ssid = "Vask-2.4G-Ka7g";
const char* password = "wish1975";

const unsigned int buttonPin = 2;

const unsigned int debounceDelay = 50;
unsigned long lastDebounceTime = 0;
unsigned long firstPressTime = 0;

bool pressedOnce = false;
bool pressed = false;

int lastButtonState = LOW;

int clickCounter = 0;

unsigned int singleCounter = 0;
unsigned int doubleCounter = 0;
unsigned int tripleCounter = 0;

WebServer server(80);

String text= "";

const String page PROGMEM = "<head>"
            " <script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.4.1/jquery.min.js\"></script>"
            " </head>"
            " <body>"
            " <h3>Single Clicks: <span id=\"singleClick\">""</span></h3>\r\n"
            " <h3>Double Clicks: <span id=\"doubleClick\">""</span></h3>\r\n"
            " <h3>Triple Clicks: <span id=\"tripleClick\">""</span></h3>\r\n"
            " <script>\r\n"
            " $(document).ready(function(){\r\n"
            " setInterval(getData,500);\r\n"
            " function getData(){\r\n"
            " $.ajax({\r\n"
            "  type:\"GET\",\r\n"
            "  url:\"data\",\r\n"
            "  success: function(data){\r\n"
            "  var s = data.split(\'-\');\r\n"
            "  $('#singleClick').html(s[0]);\r\n"
            "  $('#doubleClick').html(s[1]);\r\n"
            "  $('#tripleClick').html(s[2]);\r\n"
            "}\r\n"
            "}).done(function() {\r\n"
            "  console.log('ok');\r\n"
            "})\r\n"
            "}\r\n"
            "});"
            "</script>\r\n"
            "</body>";



void setup() {
  pinMode(buttonPin, INPUT);
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  server.on("/data", [](){ 
    text = String(singleCounter);
    text += "-";
    text += String(doubleCounter);
    text += "-";
    text += String(tripleCounter);
    Serial.println(text);
    server.send(200, "text/plain", text);
  });

  server.on("/", []() {
   server.send(200, "text/html", page);
  });

  server.begin();
  Serial.println("HTTP server started");
}


void loop() {
  int reading = digitalRead(buttonPin);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay){
    if(reading == HIGH && !pressed){
      if(!pressedOnce){
        pressedOnce = true;
        firstPressTime = millis();
      }
      pressed = true;
    }
    else if(reading == LOW && pressed){
      clickCounter++;
      pressed = false;
    }
 }

  if (((millis() - firstPressTime) > 1100) && pressedOnce){
    if(clickCounter == 1){
      singleCounter++;
      Serial.print("Single Clicks: ");
      Serial.print(singleCounter);
    }
    else if(clickCounter == 2){
      doubleCounter++;
      Serial.print("Double Clicks: ");
      Serial.print(doubleCounter);
    }
    else if(clickCounter == 3){
      tripleCounter++;
      Serial.print("Triple Clicks: ");
      Serial.print(tripleCounter);
    }
    clickCounter = 0;
    pressedOnce = false;
  }

  lastButtonState = reading;

  server.handleClient();
}

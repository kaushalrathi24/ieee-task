#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

const char* ssid = "Vask-2.4G-Ka7g";
const char* password = "wish1975";

const int led = 13;

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

IPAddress local_ip(192,168,1,174);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

// HTML web page to handle 2 input fields (threshold_input, enable_arm_input)
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>Temperature Threshold Output Control</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
  <h3>Single Clicks: %SINGLE%</h3>
  <h3>Double Clicks: %DOUBLE%</h3>
  <h3>Triple Clicks: %TRIPLE%</h3>
</body></html>)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

AsyncWebServer server(80);

// Replaces placeholder with DS18B20 values
String processor(const String& var){
  //Serial.println(var);
  if(var == "SINGLE"){
    return String(singleCounter);
  }
  else if(var == "DOUBLE"){
    return String(doubleCounter);
  }
  else if(var == "TRIPLE"){
    return String(tripleCounter);
  }
  return String();
}


void setup() {
  pinMode(buttonPin, INPUT);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Failed!");
    return;
  }
  Serial.println();
  Serial.print("ESP IP Address: http://");
  Serial.println(WiFi.localIP());

  
  // Send web page to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Receive an HTTP GET request at <ESP_IP>/get?threshold_input=<inputMessage>&enable_arm_input=<inputMessage2>
  //server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    // GET threshold_input value on <ESP_IP>/get?threshold_input=<inputMessage>
  //  request->send(200, "text/html", "HTTP GET request sent to your ESP.<br><a href=\"/\">Return to Home Page</a>");
 // });
  server.onNotFound(notFound);
  server.begin();
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

}

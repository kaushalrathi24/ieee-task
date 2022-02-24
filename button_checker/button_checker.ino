const unsigned int buttonPin = 2;

const unsigned int debounceDelay = 50;
unsigned long lastDebounceTime = 0;
unsigned long firstPressTime = 0;

bool pressedOnce = false;
bool pressed = false;

int lastButtonState = LOW;

int clickCounter = 0;

void setup() {
  pinMode(buttonPin, INPUT);
  Serial.begin(9600);

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
      Serial.println("1");
    }
    else if(clickCounter == 2){
      Serial.println("2");
    }
    else if(clickCounter == 3){
      Serial.println("3");
    }
    clickCounter = 0;
    pressedOnce = false;
  }

  lastButtonState = reading;

}

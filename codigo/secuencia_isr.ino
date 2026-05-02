const int btnPin = 4;
const int pwmPin = 5;
const int ledPins[5] = {12, 14, 27, 26, 25};

hw_timer_t * timer = NULL;
volatile unsigned long timer_ms = 0;

void IRAM_ATTR onTimer() {
  timer_ms++;
}

int estadoActual = 1;
int lastBtnState = HIGH;
int btnState;
unsigned long lastDebounce = 0;

int pwmVal = 0;
int fadeStep = 5;
unsigned long lastFade = 0;

uint8_t ledMask = 0b00000001;
int direccion = 1;
unsigned long lastLedMove = 0;

void setup() {
  Serial.begin(115200);

  pinMode(btnPin, INPUT_PULLUP);
  pinMode(pwmPin, OUTPUT);
  for(int i = 0; i < 5; i++){
    pinMode(ledPins[i], OUTPUT);
  }


  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 1000, true);
  timerAlarmEnable(timer);
}

void loop() {
  int lectura = digitalRead(btnPin);
  
  if (lectura != lastBtnState) {
    lastDebounce = timer_ms;
  }

  if ((timer_ms - lastDebounce) > 50) {
    if (lectura != btnState) {
      btnState = lectura;
      
      if (btnState == LOW) { 
        estadoActual = (estadoActual == 1) ? 2 : 1;

        analogWrite(pwmPin, 0);
        for(int i = 0; i < 5; i++) {
          digitalWrite(ledPins[i], LOW);
        }
      }
    }
  }
  lastBtnState = lectura;
  
  if (estadoActual == 1) {
    if (timer_ms - lastFade >= 15) {
      lastFade = timer_ms;

      analogWrite(pwmPin, pwmVal);
      Serial.print("PWM: ");
      Serial.println(pwmVal);

      pwmVal += fadeStep;
      
      if (pwmVal <= 0 || pwmVal >= 255) {
        fadeStep = -fadeStep; 
      }
    }
  } 
  else if (estadoActual == 2) {
    if (timer_ms - lastLedMove >= 500) {
      lastLedMove = timer_ms;

      for (int i = 0; i < 5; i++) {
        if ((ledMask >> i) & 0x01) {
          digitalWrite(ledPins[i], HIGH);
        } else {
          digitalWrite(ledPins[i], LOW);
        }
      }

      if (direccion == 1) {
        ledMask = ledMask << 1;
        if (ledMask == 0b00010000) direccion = -1;
      } else {
        ledMask = ledMask >> 1;
        if (ledMask == 0b00000001) direccion = 1;
      }
    }
  }
}

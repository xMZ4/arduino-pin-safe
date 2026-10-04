#include <Keypad.h>
#include <Servo.h>
#include <Adafruit_LiquidCrystal.h>

const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
Servo lockServo;
Adafruit_LiquidCrystal lcd(0);

const byte redLed = 11;
const byte greenLed = 12;
const byte buzzer = 13;
const byte powerButton = A0;

const byte maxAttempts = 3;
const unsigned long blockDuration = 30000;
const unsigned long debounceDuration = 50;
const unsigned long errorDuration = 1500;

String correctPin = "1234";
String enteredPin = "";

bool powered = false;
bool unlocked = false;
bool blocked = false;
bool showingError = false;

bool lastButtonReading = HIGH;
bool stableButtonState = HIGH;

byte wrongAttempts = 0;

unsigned long buttonChangedAt = 0;
unsigned long blockStarted = 0;
unsigned long errorStarted = 0;

int lastSeconds = -1;

void showPinScreen() 
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Iveskite PIN:");
  lcd.setCursor(0, 1);
}

void showCountdown() 
{
  unsigned long elapsed = millis() - blockStarted;

  if (elapsed >= blockDuration) 
  {
    return;
  }

  int secondsLeft = (blockDuration - elapsed + 999) / 1000;

  if (secondsLeft != lastSeconds) 
  {
    lastSeconds = secondsLeft;
    lcd.setCursor(0, 1);
    lcd.print("Liko: ");
    lcd.print(secondsLeft);
    lcd.print(" s   ");
  }
}

void showBlockScreen()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Uzblokuota!");
  lastSeconds = -1;
  showCountdown();
}

void lockSafe() 
{
  lockServo.write(0);
  digitalWrite(redLed, HIGH);
  digitalWrite(greenLed, LOW);

  unlocked = false;
  showingError = false;
  enteredPin = "";

  showPinScreen();
}

void setPower(bool turnOn) 
{
  powered = turnOn;
  unlocked = false;
  showingError = false;
  enteredPin = "";

  lockServo.write(0);
  noTone(buzzer);

  digitalWrite(greenLed, LOW);

  if (powered) 
  {
    digitalWrite(redLed, HIGH);
  }
  else 
  {
    digitalWrite(redLed, LOW);
  }

  lcd.clear();

  if (powered) 
  {
    lcd.setBacklight(HIGH);
  }
  else 
  {
    lcd.setBacklight(LOW);
  }

  if (powered) 
  {
    if (blocked)
    {
      showBlockScreen();
    }
    else 
    {
      showPinScreen();
    }
  }
}

void checkPowerButton() 
{
  bool reading = digitalRead(powerButton);

  if (reading != lastButtonReading) 
  {
    buttonChangedAt = millis();
    lastButtonReading = reading;
  }

  if (millis() - buttonChangedAt >= debounceDuration) 
  {
    if (reading != stableButtonState)
    {
      stableButtonState = reading;

      if (stableButtonState == LOW) 
      {
        setPower(!powered);
      }
    }
  }
}

void startBlock() 
{
  blocked = true;
  showingError = false;
  blockStarted = millis();
  enteredPin = "";

  tone(buzzer, 200, 1500);
  showBlockScreen();
}

void setup() 
{
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(powerButton, INPUT_PULLUP);

  lockServo.attach(10);

  lcd.begin(16, 2);

  setPower(false);
}

void loop() 
{
  if (blocked && millis() - blockStarted >= blockDuration) {
    blocked = false;
    wrongAttempts = 0;
    enteredPin = "";

    if (powered) 
    {
      showPinScreen();
    }
  }

  checkPowerButton();

  char key = keypad.getKey();

  if (!powered) 
  {
    return;
  }

  if (blocked) 
  {
    showCountdown();
    return;
  }

  if (showingError) 
  {
    if (millis() - errorStarted >= errorDuration) 
    {
      showingError = false;
      showPinScreen();
    }

    return;
  }

  if (!key) 
  {
    return;
  }

  tone(buzzer, 2000, 40);

  if (unlocked) 
  {
    if (key == 'D')
    {
      lockSafe();
    }

    return;
  }

  if (key == '*') 
  {
    enteredPin = "";
    showPinScreen();
  }
  else if (key == '#') {
    if (enteredPin.length() == 0) 
    {
      return;
    }

    if (enteredPin == correctPin) 
    {
      lockServo.write(90);
      digitalWrite(redLed, LOW);
      digitalWrite(greenLed, HIGH);

      unlocked = true;
      wrongAttempts = 0;
      enteredPin = "";

      tone(buzzer, 1200, 250);

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Atrakinta!");
      lcd.setCursor(0, 1);
      lcd.print("D - uzrakinti");
    }
    else 
    {
      wrongAttempts++;
      enteredPin = "";

      if (wrongAttempts >= maxAttempts) 
      {
        startBlock();
      }
      else 
      {
        tone(buzzer, 300, 350);

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Klaidingas PIN!");
        lcd.setCursor(0, 1);
        lcd.print("Liko bandymu: ");
        lcd.print(maxAttempts - wrongAttempts);

        showingError = true;
        errorStarted = millis();
      }
    }
  }
  else if (key >= '0' && key <= '9') 
  {
    if (enteredPin.length() < 5) 
    {
      enteredPin += key;
      lcd.print('*');
    }
  }
}

#include <Keypad.h>

const byte rows = 4; //four rows
const byte cols = 4; //three columns
char keys[rows][cols] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[rows] = {4, 5, 6, 7}; //connect to the row pinouts of the keypad
byte colPins[cols] = {8, 9, 10, 11}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, rows, cols );

long passwordWaitTime = 100000; // 100s time for password entering
int blinkLEDDelay = 100; // 100ms time gap to blink LED
String correctPassword = "142857";

int greenLEDPin = 12;
int redLEDPin = 13;

void setup() {
  // put your setup code here, to run once:
  pinMode(greenLEDPin, OUTPUT);
  pinMode(redLEDPin, OUTPUT);
  operateLEDs('g', 0);
  operateLEDs('r', 0);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  char key;
  String password;
  int passwordModeStartTime;

  key = keypad.getKey();
  if (key != NO_KEY)
  {
    Serial.println(key);
  }
  if (key == 'A')
  {
    Serial.println("Password mode");
    operateLEDs('g', 1); // Light up green LED to signal password mode
    passwordModeStartTime = millis();
    password = "";
    while (true)
    {
      while (key != '#' && key != '*' && key != 'D') // '#' means enter, '*' means clear, 'D' means quit this mode
      {
        key = keypad.getKey();
        if (key != NO_KEY)
        {
          Serial.println(key);
        }
        if (key != NO_KEY)
        {
          if (key >= '0' && key <= '9')
          {
            operateLEDs('g', 5); // Blink green LED once to signal one digit entered
            password.concat({key});
          }
        }
        if (millis() - passwordModeStartTime > passwordWaitTime) // Timeout
        {
          break;
        }
      }
      if (key == '#') // Enter password
      {
        if (password == correctPassword) // Good password
        {
          Serial.println("Good password");
          operateLEDs('g', 3); // Blink green LED to signal good password
          operateLEDs('g', 0);
          openDoor();
          password = "";
          break;
        }
        else // Bad password
        {
          Serial.println("Bad password");
          operateLEDs('r', 2); // Blink red LED to signal bad password
          password = "";
          key = 'A';
        }
      }
      else if (key == '*') // Clear password
      {
        Serial.println("Clear password");
        operateLEDs('r', 4); // Blink red LED once to signal password clear
        password = "";
        key = 'A';
      }
      else if (key == 'D') // Quit password mode
      {
        Serial.println("Quit password mode");
        operateLEDs('g', 0); // Turn off green LED to signal quiting password mode
        password = "";
        break;
      }
      if (millis() - passwordModeStartTime > passwordWaitTime) // Timeout
      {
        Serial.println("Password timeout");
        operateLEDs('g', 0);
        password = "";
        break;
      }
    }
  }
}

void operateLEDs(char color, int mode)
{
  int i;
  int LEDPin;
  
  if (color == 'g')
  {
    LEDPin = greenLEDPin;
  }
  else if (color == 'r')
  {
    LEDPin = redLEDPin;
  }
  if (mode == 0) // Turn off LED
  {
    digitalWrite(LEDPin, LOW);
  }
  else if (mode == 1) // Turn on LED
  {
    digitalWrite(LEDPin, HIGH);
  }
  else if (mode == 2) //  Blink LED four times, leave it off
  {
    for (i = 0; i < 4; i++)
    {
      digitalWrite(LEDPin, HIGH);
      delay(blinkLEDDelay);
      digitalWrite(LEDPin, LOW);
      delay(blinkLEDDelay);
    }
  }
  else if (mode == 3) //  Blink LED four times, leave it on
  {
    for (i = 0; i < 4; i++)
    {
      digitalWrite(LEDPin, LOW);
      delay(blinkLEDDelay);
      digitalWrite(LEDPin, HIGH);
      delay(blinkLEDDelay);
    }
  }
  else if (mode == 4) //  Blink LED once, leave in off
  {
    digitalWrite(LEDPin, HIGH);
    delay(blinkLEDDelay);
    digitalWrite(LEDPin, LOW);
    delay(blinkLEDDelay);
  }
  else if (mode == 5) //  Blink LED once, leave in on
  {
    digitalWrite(LEDPin, LOW);
    delay(blinkLEDDelay);
    digitalWrite(LEDPin, HIGH);
    delay(blinkLEDDelay);
  }
}

void openDoor()
{
  Serial.println("Opening door");
}


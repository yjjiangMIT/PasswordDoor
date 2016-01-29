#include <Keypad.h>
#include <Servo.h>

const byte rows = 4;
const byte cols = 4;
char keys[rows][cols] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[rows] = {4, 5, 6, 7}; //connect to the row pinouts of the keypad.
byte colPins[cols] = {8, 9, 10, 11}; //connect to the column pinouts of the keypad.
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, rows, cols );

Servo servo;

long passwordWaitTime = 600000; // 60s time for password entering.
int blinkLEDDelay = 100; // 100ms time gap to blink LED.
int openDoorDelay = 500; // 0.5s time to keep door open.
int relayDelay = 500; // 0.5s time for relay to actuate.
int servoDelay = 3000; // 3s time for servo to actuate.
int doorbellPulseDelay = 500; // 0.5s time between doorbell rings.

String correctPassword = "142857";

int greenLEDPin = 12;
int redLEDPin = 13;
int relayPin = 3;
int servoPin = 2;
int buzzerPin = A0;

int lockAngle = 180;
int openAngle = 0;

void setup() {
  // LED part
  pinMode(greenLEDPin, OUTPUT);
  pinMode(redLEDPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  operateLEDs('g', 0);
  operateLEDs('r', 0);

  // Servo part
  pinMode(relayPin, OUTPUT);
  servo.attach(servoPin);

  // Serial part
  Serial.begin(9600);
}

void loop() {
  char key;
  String password;
  String instruction;
  unsigned long passwordModeStartTime;
  
  if (Serial.available()) // Computer control part.
  {
    instruction = Serial.readStringUntil('\n');
    if (instruction[0] == 'O') // Receive a open-the-door command.
    {
      openDoor();  
    }
    else if (instruction[0] == 'B') // Receive a ring-the-bell command.
    {
      doorbellMode();
    }
    else if (instruction[0] == 'P') // Receive a change-password command.
    {
      correctPassword = instruction.substring(1);
    }
  }
  else // Keypad control part.
  {
    key = keypad.getKey();
    /*
    if (key != NO_KEY)
    {
      Serial.println(key);
    }
    */
    if (key == 'A') // Enter password mode.
    {
      // Serial.println("Password mode");
      passwordMode();
    }
    else if (key == 'B') // Enter doorbell mode.
    {
      // Serial.println("Doorbell mode");
      doorbellMode();
    }
  }
}

void passwordMode()
{
  char key = 'A';
  String enteredPassword;
  unsigned long passwordModeStartTime = millis();
  
  operateLEDs('g', 1); // Light up green LED to signal password mode.
  while (true)
  {
    enteredPassword = "";
    while (key != '#' && key != '*' && key != 'B' && key != 'D') // '#' means enter, '*' means clear, 'B' means doorbell mode, 'D' means quit this mode.
    {
      key = keypad.getKey();
      /*
      if (key != NO_KEY)
      {
        Serial.println(key);
      }
      */
      if (key != NO_KEY)
      {
        if (key >= '0' && key <= '9')
        {
          operateLEDs('g', 5); // Blink green LED once to signal one digit entered.
          enteredPassword.concat({key});
        }
      }
      if (abs(millis() - passwordModeStartTime) > passwordWaitTime) // Password timeout.
      {
        break;
      }
    }
    if (key == '#') // Enter password.
    {
      if (enteredPassword == correctPassword) // Good password.
      {
        // Serial.println("Good password");
        operateLEDs('g', 3); // Blink green LED to signal good password.
        operateLEDs('g', 0);
        openDoor();
        break;
      }
      else // Bad password
      {
        // Serial.println("Bad password");
        operateLEDs('r', 2); // Blink red LED to signal bad password.
        key = 'A';
      }
    }
    else if (key == '*') // Clear password
    {
      // Serial.println("Clear password");
      operateLEDs('r', 4); // Blink red LED once to signal password clear.
      key = 'A';
    }
    else if (key == 'B') // Quit password mode and enter doorbell mode.
    {
      // Serial.println("Quit password mode");
      operateLEDs('g', 0); // Turn off green LED to signal quiting password mode.
      doorbellMode();
      break;
    }
    else if (key == 'D') // Quit password mode.
    {
      // Serial.println("Quit password mode");
      operateLEDs('g', 0); // Turn off green LED to signal quiting password mode.
      break;
    }
    if (abs(millis() - passwordModeStartTime) > passwordWaitTime) // Password timeout.
    {
      // Serial.println("Password timeout");
      operateLEDs('g', 0);
      break;
    }
  }
}

void doorbellMode()
{
  operateLEDs('r', 1);
  for (int i = 0; i < 3; i++)
  {
    digitalWrite(buzzerPin, HIGH);
    delay(doorbellPulseDelay);
    digitalWrite(buzzerPin, LOW);
    delay(doorbellPulseDelay);
  }
  operateLEDs('r', 0);
}

void operateLEDs(char color, int mode)
{
  int LEDPin;
  
  if (color == 'g')
  {
    LEDPin = greenLEDPin;
  }
  else if (color == 'r')
  {
    LEDPin = redLEDPin;
  }
  if (mode == 0) // Turn off LED.
  {
    digitalWrite(LEDPin, LOW);
  }
  else if (mode == 1) // Turn on LED.
  {
    digitalWrite(LEDPin, HIGH);
  }
  else if (mode == 2) //  Blink LED four times, leave it off.
  {
    for (int i = 0; i < 4; i++)
    {
      digitalWrite(LEDPin, HIGH);
      delay(blinkLEDDelay);
      digitalWrite(LEDPin, LOW);
      delay(blinkLEDDelay);
    }
  }
  else if (mode == 3) //  Blink LED four times, leave it on.
  {
    for (int i = 0; i < 4; i++)
    {
      digitalWrite(LEDPin, LOW);
      delay(blinkLEDDelay);
      digitalWrite(LEDPin, HIGH);
      delay(blinkLEDDelay);
    }
  }
  else if (mode == 4) //  Blink LED once, leave in off.
  {
    digitalWrite(LEDPin, HIGH);
    delay(blinkLEDDelay);
    digitalWrite(LEDPin, LOW);
    delay(blinkLEDDelay);
  }
  else if (mode == 5) //  Blink LED once, leave in on.
  {
    digitalWrite(LEDPin, LOW);
    delay(blinkLEDDelay);
    digitalWrite(LEDPin, HIGH);
    delay(blinkLEDDelay);
  }
}

void openDoor()
{
  // Serial.println("Opening door");
  digitalWrite(relayPin, HIGH);
  delay(relayDelay);
  operateLEDs('g', 2);
  servo.write(openAngle);
  delay(servoDelay);
  delay(openDoorDelay);
  // Serial.println("Closing door");
  operateLEDs('r', 2);
  servo.write(lockAngle);
  delay(servoDelay);
  digitalWrite(relayPin, LOW);
  delay(relayDelay);
}

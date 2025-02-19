#include <Servo.h>

#define SERVO_PIN 9  // Der Pin, an dem der Servo angeschlossen ist
#define HALL_SENSOR_PIN A0 // Pin für den Hall-Sensor
#define BUTTON_PIN 2 // Pin für den Taster
#define DEFAULT_STEP_DELAY 37 // Standardzeit in Millisekunden zum Drehen eines Flaps (gerundet)
#define DEFAULT_SPEED 100 // Standardgeschwindigkeit des Servos (Stillstand)

Servo myServo;
int stepDelay = DEFAULT_STEP_DELAY; // Variable zur Anpassung der Drehgeschwindigkeit
int servoSpeed = DEFAULT_SPEED; // Variable zur Anpassung der Geschwindigkeit (90 = Stillstand)
bool buttonPressed = false; // Zustand des Tasters

// Zeichenmap: Index entspricht der Position auf dem Split-Flap
const char charMap[] = " ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.:";
const int numFlaps = sizeof(charMap) - 1; // Anzahl der Flaps

// Funktion zum Finden der Position eines Zeichens
int findPosition(char c) {
    for (int i = 0; i < numFlaps; i++) {
        if (charMap[i] == c) return i;
    }
    return 0; // Falls Zeichen nicht gefunden wird, nutze Leerzeichen
}

void moveToCharacter(int currentPos, int targetPos) {
    int steps = (targetPos - currentPos + numFlaps) % numFlaps;
    myServo.write(servoSpeed); // Setze den Servo auf die gewählte Geschwindigkeit
    delay(steps * stepDelay); // Warte entsprechend der Anzahl der Schritte
    myServo.write(90);  // Stoppe den Servo
    delay(10); // Kurze Pause
}

void resetToHome() {
    Serial.println("ResetHome");
    myServo.write(servoSpeed);
    while (analogRead(HALL_SENSOR_PIN) > 500) { // Warte, bis Hall-Sensor auslöst
        delay(stepDelay);
    }
    myServo.write(90); // Stoppe den Servo
    Serial.println("Resethome fertig");
}

void setFlapText(String text) {
    int currentPos = 0; // Start immer bei Leerzeichen
    resetToHome(); // Setze die Flaps auf die Leerzeichen-Position
    for (int i = 0; i < text.length(); i++) {
        int targetPos = findPosition(text[i]);
        moveToCharacter(currentPos, targetPos);
        currentPos = targetPos; // Speichere die neue Position
        delay(3000); // Halte das Zeichen für 3 Sekunden
    }
}

void setStepDelay(int newDelay) {
    stepDelay = newDelay;
}

void setServoSpeed(int newSpeed) {
    servoSpeed = newSpeed;
}

void setup() {
  Serial.begin(115200);
  Serial.println("HelloWorld");
    myServo.attach(SERVO_PIN);
    pinMode(HALL_SENSOR_PIN, INPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    myServo.write(90); // Setze den Servo auf Stillstand
    delay(1000); // Warte auf Initialisierung
    resetToHome();
}

void loop() {
    if (digitalRead(BUTTON_PIN) == LOW) { // Warte auf Tastendruck
        delay(50); // Entprellung
        if (digitalRead(BUTTON_PIN) == LOW) {
            setFlapText("ABC"); // Beispieltext ausgeben
            while (digitalRead(BUTTON_PIN) == LOW); // Warte, bis der Taster losgelassen wird
            delay(50); // Entprellung
        }
        Serial.println("Fertig");
    }
}

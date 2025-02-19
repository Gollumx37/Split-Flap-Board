#include <Servo.h>

Servo myServo;

// Pinbelegung
const int hallPin = A0;           // Hall-Sensor an Analogpin
const int servoPin = 9;           // Servo-Signalpin
const int button1Pin = 2;         // Taster 1-Pin
const int button2Pin = 3;         // Taster 2-Pin

// Schwellenwerte für Hysterese
const int restValue = 512;        // Ruhewert (ca. 2,5V)
const int thresholdOffset = 220;  // Abweichung vom Ruhewert

const int upperThreshold = restValue + thresholdOffset;  // Oberer Schwellenwert
const int lowerThreshold = restValue - thresholdOffset;  // Unterer Schwellenwert

bool servoActive = false;         // Zustandsvariable für Taster 1
unsigned long startTime = 0;      // Startzeit der Messung
unsigned long elapsedTime = 0;    // Gemessene Zeit

void setup() {
  Serial.begin(115200);           // Serielle Ausgabe für Zeitmessung
  pinMode(button1Pin, INPUT_PULLUP); // Taster 1 mit internem Pull-Up
  pinMode(button2Pin, INPUT_PULLUP); // Taster 2 mit internem Pull-Up
  pinMode(hallPin, INPUT);           // Hall-Sensor als Eingang
  myServo.attach(servoPin);          // Servo an Pin 9 anschließen
  myServo.write(90);                 // Servo bleibt anfangs stehen
  myServo.detach();
}

void loop() {
  // Taster 1: Startet den Servo, bis der Hall-Sensor auslöst
  if (digitalRead(button1Pin) == LOW && !servoActive) {
    servoActive = true;  // Servo aktivieren
    startTime = millis();  // ⏱ Zeitmessung starten
    
    myServo.attach(9);
    myServo.write(100);   // Servo dreht mit Tempo 100
  }

  // Stoppen durch Hall-Sensor
  if (servoActive) {
    int hallValue = analogRead(hallPin);
    if (hallValue > upperThreshold || hallValue < lowerThreshold) {
      elapsedTime = millis() - startTime;  // ⏱ Zeitmessung beenden
      Serial.println(elapsedTime);
      
      myServo.write(90);  // Servo stoppen
      myServo.detach();
      servoActive = false; // Servo deaktivieren
    }
  }

  // Taster 2: Servo dreht nur, solange der Taster gedrückt wird
  if (digitalRead(button2Pin) == LOW) {
    myServo.attach(9);
    myServo.write(100);   // Servo dreht mit Tempo 100
  } else if (!servoActive) { // Servo bleibt stehen, wenn Taster 2 losgelassen wird
    myServo.write(90);
    myServo.detach();
  }
}

#include <Servo.h>

Servo myServo;

// Pinbelegung
const int hallPin = A0;           // Hall-Sensor an Analogpin
const int servoPin = 9;           // Servo-Signalpin
const int HallButton = 2;         // Taster 1-Pin
const int PushButton = 3;         // Taster 2-Pin
const int SteuerButton = 4;       // Taster 3-Pin
const int Wortbutton = 5;         // Taster 4-Pin

// Schwellenwerte für Hysterese
const int restValue = 512;        // Ruhewert (ca. 2,5V)
const int thresholdOffset = 200;  // Abweichung vom Ruhewert

// Geschwindigkeit
const int v = 100;
const int turningTime = 20*40.2;      // Drehzeit in Millisekunden (34ms sieht bei Tempo 100 grade gut aus)

const int upperThreshold = restValue + thresholdOffset;  // Oberer Schwellenwert
const int lowerThreshold = restValue - thresholdOffset;  // Unterer Schwellenwert

bool servoActive = false;         // Zustandsvariable für Taster 1
int Button3Mode = 1;

int lastButtonState = HIGH;
int lastWortButtonState = HIGH;

// Text
const String flapAbfolge = " ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.:";
String Beispielwort = "HELLO WORLD";

void setup() {
  Serial.begin(115200);
  pinMode(HallButton, INPUT_PULLUP); // Taster 1 mit internem Pull-Up
  pinMode(PushButton, INPUT_PULLUP); // Taster 2 mit internem Pull-Up
  pinMode(SteuerButton, INPUT_PULLUP); // Taster 3 mit internem Pull-Up
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(hallPin, INPUT);           // Hall-Sensor als Eingang
  myServo.attach(servoPin);          // Servo an Pin 9 anschließen
  myServo.write(90);                 // Servo bleibt anfangs stehen
  myServo.detach();
}

void loop() {
  // Taster 1: Startet den Servo, bis der Hall-Sensor auslöst
  if (digitalRead(HallButton) == LOW && !servoActive) {
    servoActive = true;           // Servo aktivieren
    myServo.attach(9);
    myServo.write(v);             // Servo dreht mit Tempo v
  }

  // Stoppen durch Hall-Sensor
  if (servoActive) {
    int hallValue = analogRead(hallPin);
    if (hallValue > upperThreshold || hallValue < lowerThreshold) {
      myServo.write(90);          // Servo stoppen
      myServo.detach();
      servoActive = false;        // Servo deaktivieren
    }
  }

  // Taster 2: Servo dreht nur, solange der Taster gedrückt wird
  if (digitalRead(PushButton) == LOW) {
    myServo.attach(9);
    myServo.write(v);             // Servo dreht mit Tempo v
  } else if (!servoActive) {      // Servo bleibt stehen, wenn Taster 2 losgelassen wird
    myServo.write(90);
    myServo.detach();
  }

  //Taster 3: Wechseln zwischen Zeitgesteuert Servo drehen und Reseten über Magnet
  int buttonState = digitalRead(SteuerButton);

  if (digitalRead(SteuerButton) == LOW && !servoActive) {

    servoActive = true;

    if(Button3Mode == 1){
      Button3Mode = 3;
      Serial.println("Mode changed to 3");
      resetStellung();
    }
    else if (Button3Mode == 3){
      Button3Mode = 1;
      Serial.println("Mode changed to 1");
      zeitgesteuertesDrehen();
    }  
  }
  lastButtonState = buttonState; // Neuen Zustand speichern

//Taster 4: läuft ein eingegebenes Wort Buchstabe für Buchstabe ab.

int wortButtonState = digitalRead(Wortbutton);

  if (digitalRead(Wortbutton) == LOW && !servoActive && false) {
  servoActive = true;
  resetStellung();
  delay(50);

    for(int i=0; i<Beispielwort.length(); i++ ){
      char aktuellerBuchstabe = Beispielwort.charAt(i);
      int flapPosition = Beispielwort.indexOf(aktuellerBuchstabe);
      vorgegebenesDrehen(flapPosition*34);
      delay(50);
      resetStellung();
      delay(50);
    }
    servoActive = false;
  }  
  lastWortButtonState = wortButtonState; // Neuen Zustand speichern
}

void zeitgesteuertesDrehen(){
  servoActive = true;
  myServo.attach(9);
  myServo.write(v);             // Servo dreht mit Tempo v
  delay(turningTime);
  myServo.write(90);
  myServo.detach();
  servoActive = false;
}

void vorgegebenesDrehen(int drehzeit){
  servoActive = true;
  myServo.attach(9);
  myServo.write(v);             // Servo dreht mit Tempo v
  delay(drehzeit);
  myServo.write(90);
  myServo.detach();
  servoActive = false;
}

void resetStellung(){
  servoActive = true;           // Servo aktivieren
    myServo.attach(9);
    myServo.write(v);             // Servo dreht mit Tempo v
}

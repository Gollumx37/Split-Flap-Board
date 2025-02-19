#define LED_GPIO_NUM 4

void setup() {
    pinMode(LED_GPIO_NUM, OUTPUT);
}

void loop() {
    digitalWrite(LED_GPIO_NUM, HIGH);  // LED AN
    delay(1000);
    digitalWrite(LED_GPIO_NUM, LOW);   // LED AUS
    delay(1000);
}

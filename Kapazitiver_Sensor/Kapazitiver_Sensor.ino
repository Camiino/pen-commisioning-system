const int sensorPin = 1;  // GPIO1 (ADC1_CH0 auf ESP32-C6)

// Gleitender Durchschnitt
const int numReadings = 30;
unsigned long readings[numReadings];
int readIndex = 0;
unsigned long total = 0;
unsigned long averageTime = 0;

void setup() {
  Serial.begin(115200);

  // Initialisiere das Array für den gleitenden Durchschnitt
  for (int i = 0; i < numReadings; i++) {
    readings[i] = 0;
  }
}

unsigned long measureCapacity() {
  // 1. Sensor entladen
  pinMode(sensorPin, OUTPUT);
  digitalWrite(sensorPin, LOW);
  delay(1);

  // 2. Pin als Eingang setzen und Ladezeit messen
  pinMode(sensorPin, INPUT);
  unsigned long startTime = micros();
  while (digitalRead(sensorPin) == LOW && (micros() - startTime) < 1000000) {
    // Warte, bis der Pin HIGH wird (Timeout: 1s)
  }
  return micros() - startTime;
}

void loop() {
  // 1. Messung durchführen
  unsigned long deltaTime = measureCapacity();

  // 2. Gleitenden Durchschnitt berechnen
  total = total - readings[readIndex];
  readings[readIndex] = deltaTime;
  total = total + readings[readIndex];
  readIndex = (readIndex + 1) % numReadings;
  averageTime = total / numReadings;

  // 3. Ausgabe für Serial Plotter
  Serial.println(averageTime);

  delay(50);  // Kurze Pause für Stabilität
}

#include <Servo.h>

// -------- SERVOS --------
Servo servo1;
Servo servo2;
Servo servo3;

// -------- PINS --------
#define FLAME1 2
#define FLAME2 3
#define FLAME3 4

#define GAS1 A0
#define GAS2 A1
#define GAS3 A2

#define SERVO1_PIN 5
#define SERVO2_PIN 6
#define SERVO3_PIN 7

#define PUMP 8
#define FAN1 9
#define FAN2 10
#define FAN3 11

#define LED1 12
#define LED2 13
#define LED3 A3
#define BUZZER A4

int gasThreshold = 750;

// -------- TIMERS --------
unsigned long fireTime1 = 0, fireTime2 = 0, fireTime3 = 0;
unsigned long gasTime1 = 0, gasTime2 = 0, gasTime3 = 0;

unsigned long ledTime1 = 0, ledTime2 = 0, ledTime3 = 0;

const int holdTime = 5000;

// -------- GAS READ --------
int readGas(int pin) {
  int sum = 0;
  for (int i = 0; i < 5; i++) {
    sum += analogRead(pin);
    delay(5);
  }
  return sum / 5;
}

void setup() {
  Serial.begin(9600);

  pinMode(FLAME1, INPUT_PULLUP);
  pinMode(FLAME2, INPUT_PULLUP);
  pinMode(FLAME3, INPUT_PULLUP);

  pinMode(PUMP, OUTPUT);
  pinMode(FAN1, OUTPUT);
  pinMode(FAN2, OUTPUT);
  pinMode(FAN3, OUTPUT);

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  digitalWrite(PUMP, HIGH);
  digitalWrite(FAN1, HIGH);
  digitalWrite(FAN2, HIGH);
  digitalWrite(FAN3, HIGH);

  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);
  servo3.attach(SERVO3_PIN);

  servo1.write(0);
  servo2.write(0);
  servo3.write(0);
}

void loop() {

  unsigned long now = millis();

  // -------- READ --------
  int f1 = digitalRead(FLAME1);
  int f2 = digitalRead(FLAME2);
  int f3 = digitalRead(FLAME3);

  int g1 = readGas(GAS1);
  int g2 = readGas(GAS2);
  int g3 = readGas(GAS3);

  bool fire1 = (f1 == LOW);
  bool fire2 = (f2 == LOW);
  bool fire3 = (f3 == LOW);

  bool gas1 = (g1 > gasThreshold);
  bool gas2 = (g2 > gasThreshold);
  bool gas3 = (g3 > gasThreshold);

  // -------- UPDATE TIMERS --------
  if (fire1) {
    fireTime1 = now;
    ledTime1 = now;
  }
  if (fire2) {
    fireTime2 = now;
    ledTime2 = now;
  }
  if (fire3) {
    fireTime3 = now;
    ledTime3 = now;
  }

  if (gas1) {
    gasTime1 = now;
    ledTime1 = now;
  }
  if (gas2) {
    gasTime2 = now;
    ledTime2 = now;
  }
  if (gas3) {
    gasTime3 = now;
    ledTime3 = now;
  }

  // -------- ROOM 1 --------
  if (now - fireTime1 <= holdTime) {
    servo1.write(90);
    digitalWrite(FAN1, LOW);
  } else {
    servo1.write(0);

    if (now - gasTime1 <= holdTime) {
      digitalWrite(FAN1, LOW);
    } else {
      digitalWrite(FAN1, HIGH);
    }
  }

  // -------- ROOM 2 --------
  if (now - fireTime2 <= holdTime) {
    servo2.write(90);
    digitalWrite(FAN2, LOW);
  } else {
    servo2.write(0);

    if (now - gasTime2 <= holdTime) {
      digitalWrite(FAN2, LOW);
    } else {
      digitalWrite(FAN2, HIGH);
    }
  }

  // -------- ROOM 3 --------
  if (now - fireTime3 <= holdTime) {
    servo3.write(90);
    digitalWrite(FAN3, LOW);
  } else {
    servo3.write(0);

    if (now - gasTime3 <= holdTime) {
      digitalWrite(FAN3, LOW);
    } else {
      digitalWrite(FAN3, HIGH);
    }
  }

  // -------- PUMP --------
  if ((now - fireTime1 <= holdTime) ||
      (now - fireTime2 <= holdTime) ||
      (now - fireTime3 <= holdTime)) {
    digitalWrite(PUMP, LOW);
  } else {
    digitalWrite(PUMP, HIGH);
  }

  // -------- LED HOLD --------
  digitalWrite(LED1, (now - ledTime1 <= holdTime) ? HIGH : LOW);
  digitalWrite(LED2, (now - ledTime2 <= holdTime) ? HIGH : LOW);
  digitalWrite(LED3, (now - ledTime3 <= holdTime) ? HIGH : LOW);

  // -------- BUZZER --------
  if ((now - fireTime1 <= holdTime) ||
      (now - fireTime2 <= holdTime) ||
      (now - fireTime3 <= holdTime)) {
    tone(BUZZER, 1000);
  }
  else if ((now - gasTime1 <= holdTime) ||
           (now - gasTime2 <= holdTime) ||
           (now - gasTime3 <= holdTime)) {
    tone(BUZZER, 800);
    delay(150);
    noTone(BUZZER);
    delay(150);
  }
  else {
    noTone(BUZZER);
  }

  // -------- SERIAL OUTPUT --------
  Serial.println("========== LIVE DATA ==========");

  Serial.print("Room1 -> Gas: "); Serial.print(g1);
  Serial.print(" | Flame: "); Serial.print(fire1 ? "FIRE" : "SAFE");
  Serial.print(" | Fan: "); Serial.print(digitalRead(FAN1) == LOW ? "ON" : "OFF");
  Serial.print(" | LED: "); Serial.println(digitalRead(LED1));

  Serial.print("Room2 -> Gas: "); Serial.print(g2);
  Serial.print(" | Flame: "); Serial.print(fire2 ? "FIRE" : "SAFE");
  Serial.print(" | Fan: "); Serial.print(digitalRead(FAN2) == LOW ? "ON" : "OFF");
  Serial.print(" | LED: "); Serial.println(digitalRead(LED2));

  Serial.print("Room3 -> Gas: "); Serial.print(g3);
  Serial.print(" | Flame: "); Serial.print(fire3 ? "FIRE" : "SAFE");
  Serial.print(" | Fan: "); Serial.print(digitalRead(FAN3) == LOW ? "ON" : "OFF");
  Serial.print(" | LED: "); Serial.println(digitalRead(LED3));

  Serial.print("Pump: ");
  Serial.println(digitalRead(PUMP) == LOW ? "ON" : "OFF");

  Serial.println("=================================\n");

  delay(400);
}
#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int buttonPin = 8;        // Pinul pentru buton
const int ledPin = 13;          // Pinul LED-ului roșu
const int speakerPin = 6;       // Pinul difuzorului
const int sensorPin = A0;       // Pinul senzorului de umiditate

bool systemOn = false;           // Starea sistemului (pornit/oprit)
float humidity = 0;              // Variabilă pentru umiditate

unsigned long previousMillis = 0;  // Timpul anterior pentru intervalul de alarmă
const long interval = 6300;       // Intervalul de actualizare a umidității (6.3 secunde)
const long alarmInterval = 330;  // Intervalul de alarmă (330ms)
const int alarmPause = 100;       // Pauza între alarme (100ms)

void setup() {
  pinMode(buttonPin, INPUT);      // Setarea pinului butonului ca intrare
  pinMode(ledPin, OUTPUT);        // Setarea pinului LED-ului ca ieșire
  pinMode(speakerPin, OUTPUT);    // Setarea pinului difuzorului ca ieșire

  lcd.begin(16, 2);               // Inițializarea LCD-ului
  lcd.print("Sistem oprit");        // Afișarea mesajului "Sistem oprit"
}

void loop() {
  // Verificarea stării butonului (pornit/oprit)
  if (digitalRead(buttonPin) == HIGH) {
    systemOn = !systemOn;          // Inversarea stării sistemului
    delay(250);                    // Anti-rebound delay

    if (systemOn) {
      lcd.clear();
      lcd.print("Sistem On ");
      lcd.setCursor(0, 1);
      lcd.print("Reading hum...");
      delay(1500);                 // Așteptare pentru stabilizarea senzorului de umiditate
      lcd.clear();
    } else {
      lcd.clear();
      lcd.print("Sistem oprit");
      digitalWrite(ledPin, LOW);   // Stinge LED-ul
      noTone(speakerPin);          // Oprește difuzorul
      humidity = 0;                // Resetarea umidității
    }
  }

  // Verificarea dacă sistemul este pornit și intervalul pentru citirea umidității
  if (systemOn && millis() - previousMillis >= interval) {
    previousMillis = millis();       // Actualizează timpul anterior

    humidity = readHumidity();       // Citirea umidității

    // Controlul LED-ului și a difuzorului în funcție de umiditate
    if (humidity > 500) {
      digitalWrite(ledPin, HIGH);    // Aprinde LED-ul
      noTone(speakerPin);            // Oprește difuzorul
    } else {
      digitalWrite(ledPin, LOW);     // Stinge LED-ul
      unsigned long currentMillis = millis();
      // Verificăm dacă a trecut intervalul specificat pentru alarmă
      if (currentMillis - previousMillis >= alarmInterval) {
        previousMillis = currentMillis; // Actualizăm timpul ultimei alarme
        // Pornim alarma și o oprim după o perioadă specificată
        tone(speakerPin, 10000, 10);   // Porneste alarma cu o frecvență de 10 kHz
        delay(alarmPause);              // Pauză între alarme
        noTone(speakerPin);             // Oprește alarma
      }
    }

    // Afișarea umidității pe LCD
    lcd.setCursor(0, 0);
    lcd.print("Humidity: ");
    lcd.print(humidity);
    lcd.print(" %");
  }
}

float readHumidity() {
  int numReadings = 9;              // Numărul de citiri pentru medie
  float totalHumidity = 0;

  for (int i = 0; i < numReadings; i++) {
    totalHumidity += analogRead(sensorPin);
    delay(50);                       // Pauză între citiri
  }

  return totalHumidity / numReadings;
}

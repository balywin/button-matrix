#include <Arduino.h>
#include <SPI.h>
#include <Keypad.h>

// #define DEBUG_MESSAGES

// ==========================================
// 1. ПИНОВЕ И КОНФИГУРАЦИЯ НА MCP4241
// ==========================================
const int CS_PIN = 10;            // Chip Select (D10)
const byte WRITE_WIPER_0 = 0x00;  // Адрес на Wiper 1 (POT2 / P0W) за MCP4241
const byte WRITE_WIPER_1 = 0x10;  // Адрес на Wiper 2 (POT2 / P1W) за MCP4241
const byte IDLE_RESISTANCE = 127; // Максимално съпротивление при липса на натиснат бутон (Отворен верига)

const int ANALOG_PIN = A3;

// ==========================================
// 2. КОНФИГУРАЦИЯ НА 4x3 МАТРИЦАТА
// ==========================================
const byte ROWS = 4;
const byte COLS = 3;

// Карта на бутоните
char keys[ROWS][COLS] = {
  {'1', '7', '3'},
  {'6', '8', '4'},
  {'0', '9', '+'},
  {'5', '-', '2'}
};

// Използваме D3..D9 съгласно твоята конфигурация
byte rowPins[ROWS] = {3, 4, 5, 6}; // Редове (D3, D4, D5, D6)
byte colPins[COLS] = {7, 8, 9};    // Колони (D7, D8, D9)

// Инициализация на Keypad библиотеката
Keypad customKeypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

bool justReleased = true;

// ==========================================
// 3. ФУНКЦИЯ ЗА ЗАПИС В MCP4241 (POT1 / Wiper 0)
// ==========================================
static void setPotentiometer(byte level) {
  // Ограничение на резолюцията до 7 бита (0 - 127)
  if (level > 127) level = 127;

  digitalWrite(CS_PIN, LOW);
  SPIClass::transfer(WRITE_WIPER_1); // Изпращаме адрес/команда
  SPIClass::transfer(level);         // Изпращаме стойността на съпротивлението
  digitalWrite(CS_PIN, HIGH);
}

// ==========================================
// 4. МАРШРУТИЗАЦИЯ НА БУТОНИ КЪМ СТОЙНОСТИ (0-127)
// ==========================================
byte getResistanceForKey(char key) {
  switch (key) {
    case '1': return 10;   // Vol +
    case '2': return 20;   // Vol -
    case '3': return 30;   // Next Track
    case '4': return 40;   // Prev Track
    case '5': return 50;   // Play / Pause
    case '6': return 60;   // Mode / Source
    case '7': return 70;   // Mute
    case '8': return 80;   // Home
    case '9': return 90;   // Back
    case '+': return 100;  // Call Accept
    case '0': return 110;  // Call Reject
    case '-': return 120;  // Voice Assistant
    default:  return IDLE_RESISTANCE;
  }
}

// ==========================================
// 5. SETUP & LOOP
// ==========================================
void setup() {
  Serial.begin(19200);

  // Настройка на CS пина за SPI
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);
  pinMode(ANALOG_PIN, INPUT);

  // Настройка на Debounce времетраене за матрицата (в милисекунди)
  // Библиотеката Keypad използва неблокиращ софтуерен анти-реблек (Debounce)
  customKeypad.setDebounceTime(25);

  // Стартиране на SPI шината
  SPIClass::begin();

  // Начално състояние: Ненатиснат бутон (Максимално съпротивление)
  setPotentiometer(IDLE_RESISTANCE);
  Serial.println(F("Системата е готова. MCP4241 POT1 е в начално състояние."));
}

void loop() {
  // getKey() автоматично изпълнява сканиране и филтриране на дебаунс (debounce)
  const char key = customKeypad.getKey();
  const KeyState state = customKeypad.getState();

  // При натискане на нов бутон
  if (key != NO_KEY && state == PRESSED) {
    const byte resistanceVal = getResistanceForKey(key);
    setPotentiometer(resistanceVal);

#ifdef DEBUG_MESSAGES
    Serial.print(F("Button pressed: "));
    Serial.print(key);
    Serial.print(F(" -> POT1 value: "));
    Serial.print(resistanceVal);

    // Read raw 10-bit value (0 to 1023)
    int rawValue = analogRead(ANALOG_PIN);
    Serial.print(F("  A3 Raw Value: "));
    Serial.print(rawValue);
#endif    
    justReleased = true;
  }

  // При отпускане на бутон (връщане в отворена верига)
  if ((state == RELEASED) && justReleased) {
#ifdef DEBUG_MESSAGES
    Serial.print(F("  Idling..."));
    Serial.println(key);
#endif    
    setPotentiometer(IDLE_RESISTANCE);
    justReleased = false;
  }
}
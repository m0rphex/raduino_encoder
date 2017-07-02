#include <LiquidCrystal.h>
#include <Rotary.h>
#include <Wire.h>
#include <si5351.h>

Si5351 si5351;
LiquidCrystal lcd(8, 9, 10, 11, 12, 13);
const unsigned long if_freq = 12000000;
volatile unsigned long increment = 1000000;
volatile unsigned long frequency = 5000000;
Rotary r = Rotary(2, 3);
bool interrupt = false;
bool button = false;

void setup_rotary() {
  pinMode(4, INPUT_PULLUP);
  PCICR |= (1 << PCIE2);
  PCMSK2 |= (1 << PCINT18) | (1 << PCINT19) | (1 << PCINT20);
  sei();
}

void setup_dds() {
  si5351.init(SI5351_CRYSTAL_LOAD_8PF, 0, 191600);
  // si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_6MA);
  si5351.set_freq(frequency * 100, SI5351_CLK0);
}

ISR(PCINT2_vect) {
  interrupt = true;
  unsigned char result = r.process();
  if (result == DIR_CW) {
    frequency += increment;
  }
  if (result == DIR_CCW) {
    frequency -= increment;
  }

  if (digitalRead(4) == 0) {
    button = true;
  }
}

void setup() {
  Serial.begin(9600);
  setup_dds();
  setup_rotary();
  lcd.begin(16, 2);
  lcd.print(increment);
  lcd.setCursor(0, 1);
  lcd.print(frequency);
}

void loop() {
  if (interrupt) {
    interrupt = false;
    if (button) {
      button = false;
      increment + 1;
    }
    lcd.clear();
    lcd.print(increment);
    lcd.setCursor(0, 1);
    lcd.print(frequency);
    si5351.set_freq(frequency * 100, SI5351_CLK0);
  }
}

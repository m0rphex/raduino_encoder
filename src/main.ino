#include <LiquidCrystal.h>
#include <Rotary.h>
#include <Wire.h>
#include <avr/sleep.h>
#include <si5351.h>

Si5351 si5351;
LiquidCrystal lcd(8, 9, 10, 11, 12, 13);
const unsigned long if_freq = 12000000;
int inc_mode = 0;
unsigned long inc_val[5] = {10, 100, 1000, 10000, 100000};
unsigned long frequency = 5000000;
Rotary r = Rotary(2, 3);
bool interrupt = false;
bool inc_up = false;
bool inc_down = false;
bool inc_butt = false;
unsigned long deb_time = 0;

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
    inc_up = true;
  }
  if (result == DIR_CCW) {
    inc_down = true;
  }

  if (digitalRead(4) == 0 && (millis() - deb_time) > 300) {
    inc_butt = true;
    deb_time = millis();
  }
}

void setup() {
  Serial.begin(9600);
  setup_dds();
  setup_rotary();
  lcd.begin(16, 2);
  lcd.print(inc_val[inc_mode]);
  lcd.setCursor(0, 1);
  lcd.print(frequency);
}

void loop() {
  if (interrupt) {
    interrupt = false;
    if (inc_butt) {
      inc_butt = false;
      inc_mode++;
      if (inc_mode > 4) {
        inc_mode = 0;
      }
    }

    if (inc_up) {
      inc_up = false;
      frequency += inc_val[inc_mode];
    }

    if (inc_down) {
      inc_down = false;
      frequency -= inc_val[inc_mode];
    }

    lcd.clear();
    lcd.print(inc_val[inc_mode]);
    lcd.setCursor(0, 1);
    lcd.print(frequency);
    si5351.set_freq(frequency * 100, SI5351_CLK0);
  }

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
}

#include <si5351.h>
#include <LiquidCrystal.h>
#include <Wire.h>
#include <Rotary.h>

Si5351 si5351;
LiquidCrystal lcd(8,9,10,11,12,13);
const unsigned long if_freq = 12000000;
int increment = 1000;
volatile unsigned long frequency = 500000000;
Rotary r = Rotary(2, 3);
bool freq_update = false;

void setup_rotary(){
  PCICR |= (1 << PCIE2);
  PCMSK2 |= (1 << PCINT18) | (1 << PCINT19);
  sei();
}

void setup_dds(){
si5351.init(SI5351_CRYSTAL_LOAD_8PF, 0,191600);
//si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_6MA);
  si5351.set_freq(frequency, SI5351_CLK0);
}

ISR(PCINT2_vect) {
  freq_update = true;
  unsigned char result = r.process();
  if (result == DIR_NONE) {
    freq_update = false;
    // button press?
  }
  else if (result == DIR_CW) {
    frequency = frequency + 1000000;
  }
  else if (result == DIR_CCW) {
    frequency = frequency - 1000000;
  }
}

void setup()
{
  Serial.begin(9600);
  setup_dds();
  setup_rotary();
  lcd.begin(16,2);
  lcd.print(frequency/100);
}

void loop()
{
if (freq_update){
  freq_update = false;
  lcd.home();
  lcd.print(frequency/100);
  //si5351.set_freq(frequency, SI5351_CLK0);
}
}

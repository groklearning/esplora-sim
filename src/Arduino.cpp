#include <iostream>
#include "Arduino.h"
#include "global_variables.h"

// const int sleep_period = 100; // sleep period in milliseconds
// const int us_sleep_period = sleep_period * 1000;


void pinMode(int pin, int mode) {
  if (pin > NUM_PINS || pin < 0) {
    _sim::increment_counter(1);
    return;
  }
  if (mode == INPUT || mode == INPUT_PULLUP || mode == OUTPUT) {
    _sim::_device.set_pin_mode(pin, mode);
  }
  _sim::increment_counter(1);
}

void digitalWrite(int pin, byte value) {
  if (pin > NUM_PINS || pin < 0) {
    _sim::increment_counter(1);
    return;
  }
  int mode = _sim::_device.get_pin_mode(pin);
  if (mode == INPUT) {
    // if mode is input on digital write, we enable (high) or disable (low)
    // the pullup resistor
    _sim::_device.set_pullup_digwrite(pin, value);
  } else if (mode == OUTPUT) {
    _sim::_device.set_digital(pin, (value) ? HIGH : LOW);
  }
  _sim::increment_counter(4);
}

int digitalRead(int pin) {
  _sim::increment_counter(1);
  return _sim::_device.get_digital(pin);
}

void analogWrite(int pin, byte value) {
  if (!_sim::_device.digitalPinHasPWM(pin)) {
    _sim::increment_counter(1);
    return;
  }

  pinMode(pin, OUTPUT);
  if (value == 0)
    digitalWrite(pin, LOW);
  else if (value == 255)
    digitalWrite(pin, HIGH);
  else {
    // must set pwm_peroid first because high_time is a function of period
    _sim::_device.default_pwm_period(pin);
    _sim::_device.set_pwm_high_time(pin, value);
  }
  _sim::increment_counter(10);
}

int analogRead(int pin) {
  _sim::increment_counter(100);
  if (pin >= 0 && pin <= 11) pin += 18;
  return _sim::_device.get_analog(pin);
}

void analogReference(uint8_t mode __attribute__((unused))) {
  // do nothing
}

//------ Advanced I/O ----------------------
void tone(unsigned int pin, unsigned int freq) {
  if (!_sim::_device.digitalPinHasPWM(pin)) {
    _sim::increment_counter(1);
    return;
  }
  pinMode(pin, OUTPUT);
  _sim::_device.set_tone(pin, freq);
  _sim::increment_counter(1);
}

void tone(unsigned int pin, unsigned int freq, unsigned long duration) {
  _sim::increment_counter(1);
  if (!_sim::_device.digitalPinHasPWM(pin))
    return;
  tone(pin, freq);
  _sim::_device.set_countdown(pin, duration * 1000UL);
}

void noTone(unsigned int pin) {
  tone(pin, 0);
}

void shiftOut(int dataPin __attribute__((unused)), int clockPin __attribute__((unused)), int bitOrder __attribute__((unused)), int value __attribute__((unused))) {
  _sim::increment_counter(1);
  //bitOrder: which order to shift out the bits; either MSBFIRST or LSBFIRST.
}

int shiftIn(int dataPin __attribute__((unused)), int clockPin __attribute__((unused)), int bitOrder __attribute__((unused))) {
  _sim::increment_counter(1);
  //bitOrder: which order to shift out the bits; either MSBFIRST or LSBFIRST.
  return 0;
}


//------ Time ------------------------------

unsigned long millis() {
  _sim::increment_counter(1);
  unsigned long e = _sim::_device.get_micros();
  return e / 1000;
}

// Return microseconds rounded down
// to the nearest multiple of 4
unsigned long
micros() {
  _sim::increment_counter(1);
  unsigned long e = _sim::_device.get_micros();
  int rem = e % 4;
  if (rem == 0)
    return e;
  return e - rem;
}


void delay(uint32_t ms) {
  _sim::increment_counter(ms*1000);

}

void delayMicroseconds(uint32_t us) {
  _sim::increment_counter(us);
}

int map(int x, int fromLow, int fromHigh, int toLow, int toHigh) {
  _sim::increment_counter(1);
  int y;
  y = (float)(x - fromLow) / (fromHigh - fromLow) * (toHigh - toLow) + toLow;
  return y;
}


//------ Random Numbers --------------------
void randomSeed(int seed) {
  _sim::increment_counter(1);
  srand(seed);
}

long random(long upperLimit) {
  _sim::increment_counter(2);
  long x = RAND_MAX / upperLimit;
  x = long(rand() / x);
  return x;
}

long random(long lowerLimit, long upperLimit) {
  _sim::increment_counter(2);
  long interval, temp = 0;
  if (lowerLimit < upperLimit) {
    interval = upperLimit - lowerLimit;
    temp = lowerLimit + random(interval);
  }
  return temp;
}

/*
  Device.cpp - Arduino simulator Esplora board library
  Written by Owen Brasier

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 3 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "Device.h"
#include "global_variables.h"

#include <iostream>


double dmap(double x, double x1, double x2, double y1, double y2) {
  return (x - x1) * (y2 - y1) / (x2 - x1) + y1;
}

_Device::_Device() {
  _sim::send_pin_update();
  _micros_elapsed = 0;

  for (int i = 0; i < NUM_PINS; i++) {
    _pins[i]._pin = i + 1;
    _pins[i]._voltage = NAN;
    if (isAnalogPin(i))
      _pins[i]._is_analog = true;
    if (digitalPinHasPWM(i))
      _pins[i]._is_pwm = true;
  }
  float freq;
  for (const auto &elem : _pwm_frequencies) {
    freq = (1.0 / static_cast<float>(elem.second)) * 1000000.0;
    set_pwm_period(elem.first, static_cast<uint32_t>(freq));
  }

  std::array<int, 4> switches {{ CH_SWITCH_1, CH_SWITCH_2, CH_SWITCH_3, CH_SWITCH_4 }};
  // set switches to be high (active low)
  for (const auto &elem : switches)
    set_mux_voltage(elem, 1023);
  set_mux_voltage(CH_JOYSTICK_SW, 1023);
  _sim::send_pin_update();
}


void _Device::increment_counter(uint32_t us) {
  _micros_elapsed += us;
  std::lock_guard<std::mutex> lk(_m_countdown);
  for (int i = 0; i < NUM_PINS; i++) {
    if (_pins[i]._countdown > 0) {
      _pins[i]._countdown -= us;
      if (_pins[i]._countdown <= 0) {
        _pins[i]._countdown = 0;
        // timer has expired on pin i
        set_tone(i, 0);
      }
    }
  }
}

uint64_t _Device::get_micros() {
  return _micros_elapsed;
}

void _Device::set_pin_value(int pin, int value) {
  std::lock_guard<std::mutex> lk(_m_pins);
  int v;
  v = dmap(value, 0.0, 5.0, 0, 1023);
  _pins[pin]._voltage = value;
  if (_pins[pin]._is_analog) {
    _pins[pin]._value = v;
    set_analog(pin, value);
  } else {
    v =  (value >= 1.5) ? HIGH : LOW;
    set_digital(pin, v);
  }
  _pin_values[pin] = v;
  _sim::send_pin_update();
}

int _Device::get_pin_value(int pin) {
  std::lock_guard<std::mutex> lk(_m_pins);
  return  _pin_values[pin];
}

void _Device::set_mux_voltage(int pin, double value) {
  std::lock_guard<std::mutex> lk(_m_mux);
  _mux_pins[pin]._voltage = value;
  int v = dmap(value, 0.0, 5.0, 0, 1023);
  _mux_pins[pin]._value = v;
}

int _Device::get_mux_value(int pin) {
  std::lock_guard<std::mutex> lk(_m_mux);
  return _mux_pins[pin]._value;
}


void _Device::set_pin_mode(int pin, int mode) {
  std::lock_guard<std::mutex> lk(_m_modes);
  switch (mode) {
    case INPUT:
      set_pin_state(pin, GPIO_PIN_INPUT_FLOATING);
      break;
    case INPUT_PULLUP:
      set_pin_state(pin, GPIO_PIN_INPUT_UP_HIGH);
      break;
    case OUTPUT:
      set_pin_state(pin, GPIO_PIN_OUTPUT_LOW);
      break;
    default:
      return;
  }
  _pins[pin]._mode = mode;
}

int _Device::get_pin_mode(int pin) {
  std::lock_guard<std::mutex> lk(_m_modes);
  return _pins[pin]._mode;
}

void _Device::set_pin_state(int pin, PinState state) {
  std::lock_guard<std::mutex> lk(_m_states);
  _pins[pin]._state = state;
}

PinState _Device::get_pin_state(int pin) {
  std::lock_guard<std::mutex> lk(_m_states);
  return _pins[pin]._state;
}

void _Device::set_pwm_dutycycle(int pin, uint32_t a_write) {
  std::lock_guard<std::mutex> lk(_m_pwmd);
  if (!_pins[pin]._is_pwm)
    return;
  if (a_write == 0)
    set_pin_state(pin, GPIO_PIN_OUTPUT_LOW);
  set_pin_state(pin, GPIO_PIN_OUTPUT_PWM);
  uint32_t high_time = _pins[pin]._pwm_period * ((float)a_write / 255.0);
  _pins[pin]._pwm_high_time = high_time;
}

uint32_t _Device::get_pwm_dutycycle(int pin) {
  std::lock_guard<std::mutex> lk(_m_pwmd);
  return _pins[pin]._pwm_high_time;
}

void _Device::set_pwm_period(int pin, uint32_t period) {
  std::lock_guard<std::mutex> lk(_m_pwmp);
  _pwm_period[pin] = period;
}

uint32_t _Device::get_pwm_period(int pin) {
  std::lock_guard<std::mutex> lk(_m_pwmp);
  return  _pwm_period[pin];
}

void _Device::set_digital(int pin, int level) {
  std::lock_guard<std::mutex> lk(_m_pins);
  _digital_values[pin] = level;
  if (level == LOW)
    set_pin_state(pin, GPIO_PIN_OUTPUT_LOW);
  else if (level == HIGH)
    set_pin_state(pin, GPIO_PIN_OUTPUT_HIGH);
}

int _Device::get_digital(int pin) {
  std::lock_guard<std::mutex> lk(_m_pins);
  return _digital_values[pin];
}

void _Device::set_analog(int pin, int value) {
  std::lock_guard<std::mutex> lk(_m_analog);
  if (pin >= 18) // work for pin numbers as well as channel numbers
    pin -= 18;
  if (isAnalogPin(pin))
    _analog_values[pin] = value;
}

int _Device::get_analog(int pin) {
  std::lock_guard<std::mutex> lk(_m_analog);
  if (pin >= 18)
    pin -= 18;
  if (isAnalogPin(pin))
    return _analog_values[pin];
  return 0;
}

void _Device::set_tone(int pin, uint32_t freq) {
  int period = 0;
  if (freq != 0)
    period = 1000000 / freq;
  set_pwm_period(pin, period);
  set_pwm_dutycycle(pin, freq);
  _sim::send_pin_update();
}


void _Device::set_countdown(int pin, uint32_t d) {
  std::lock_guard<std::mutex> lk(_m_countdown);
  _pins[pin]._countdown = d;
}

void _Device::set_pullup_digwrite(int pin, int value) {
  PinState mode = get_pin_state(pin);
  if (value == HIGH) {
    // enable pullup
    if (mode >= GPIO_PIN_INPUT_FLOATING && mode <= GPIO_PIN_INPUT_FLOATING_HIGH) {
      set_pin_state(pin, GPIO_PIN_INPUT_UP_HIGH);
    }
  } else if (value == LOW) {
    if (mode >= GPIO_PIN_INPUT_UP_LOW && mode <= GPIO_PIN_INPUT_DOWN_HIGH)
      set_pin_state(pin, GPIO_PIN_INPUT_FLOATING);
  }
}

bool _Device::digitalPinHasPWM(int p) {
  return ((p) == 3 || (p) == 5 || (p) == 6 || (p) == 9 || (p) == 10 || (p) == 11 || (p) == 13);
}

bool _Device::isAnalogPin(int p) {
  return (p >= 0 && p <= 11);
}

namespace _sim {
// check the suspend flag, if suspend is false, then continue
// otherwise wait for the condition variable, cv_suspend
void
check_suspend() {
  std::unique_lock<std::mutex> lk(m_suspend);
  cv_suspend.wait(lk, [] {return suspend == false;});
}

// If we receive a shutdown signla
// send a final status update before
// exiting, enables fast_mode so the loop will finish
// without any delays
void
check_shutdown() {
  if (shutdown) {
    running = false;
    fast_mode = true;
    send_pin_update();
    send_updates = false;
  }
}

void
increment_counter(int us) {
  _device.increment_counter(us);
  check_suspend();
  check_shutdown();
  send_pin_update();
}
} // namespace
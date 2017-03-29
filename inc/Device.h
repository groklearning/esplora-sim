#ifndef DEVICE_H_
#define DEVICE_H_

#include <array>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <thread>
#include <algorithm>

#define SIM_SWITCH_1        1
#define SIM_SWITCH_2        2
#define SIM_SWITCH_3        3
#define SIM_SWITCH_4        4
#define SIM_SLIDER          7
#define SIM_LIGHT           8
#define SIM_TEMPERATURE     11
#define SIM_MIC             12
#define SIM_TINKERKIT_INA   13  // tinkerkit input
#define SIM_TINKERKIT_INB   14  // tinkerkit input
#define SIM_JOYSTICK_SW     15
#define SIM_JOYSTICK_X      16
#define SIM_JOYSTICK_Y      17
#define SIM_RED             5
#define SIM_GREEN           9
#define SIM_BLUE            10
#define SIM_ACCEL_X         23
#define SIM_ACCEL_Y         29
#define SIM_ACCEL_Z         24
#define SIM_BUZZER          6

#define NUM_PINS            30
#define NUM_LEDS            25
#define NUM_DIGITAL_PINS    30
#define NUM_ANALOG_PINS     12


void check_suspend();
void check_shutdown();
void increment_counter(int us);

// in Main.cpp
void send_pin_update();
void send_led_update();



enum class PinState {
  FLOATING = 0,
  INPUT,
  OUTPUT,
  INPUT_PULLUP,
  PWM_OUTPUT
};


// Led for regular leds, the 4 on the top 
// ON, L, TX, RX
class Led
{
private:
  uint8_t _number;
  uint8_t _brightness;
  bool _state;
  bool is_on() {return _state;}

public:
  Led();
  uint8_t brightness() {return _brightness;}
};

class RgbLed
{
private:
  uint8_t _red;
  uint8_t _green;
  uint8_t _blue;
  std::array<uint8_t, 3> _rgb;

public:
  RgbLed();
  std::array<uint8_t, 3> get_rgb();
  void set_red(uint8_t red) {_red = red;}
  void set_green(uint8_t green) { _green = green;}
  void set_blue(uint8_t blue) {_blue = blue;}
  void set_rgb(uint8_t red, uint8_t green, uint8_t blue);

};

// The device class stores all the information required about a device.
// It is designed to be thread-safe internally, so no external mutexs should
// be required.
class Device
{
private:
  std::array<int, NUM_PINS> _pin_values;
  std::array<int, NUM_LEDS> _led_values;
  std::array<bool, NUM_PINS> _digital_states;
  std::array<int, NUM_ANALOG_PINS> _analog_states;
  std::array<PinState, NUM_PINS> _pin_modes;
  std::array<int, NUM_PINS> _pwm_dutycycle;
  std::array<int, NUM_PINS> _pwm_period;
  uint64_t _micros_elapsed;
  std::mutex _m_device;
  std::mutex _m_micros;
  std::mutex _m_pins;
  std::mutex _m_modes;
  std::mutex _m_leds;

public:
  Device();
  void set_pin_value(int pin, int value);
  int get_pin_value(int pin);
  std::array<int, NUM_PINS> get_all_pins();
  void zero_all_pins();
  void set_pin_mode(int pin, PinState mode);
  PinState get_pin_mode(int pin);
  void set_pwm_dutycycle(int pin, uint32_t dutycycle);
  uint32_t get_pwm_dutycycle(int pin);
  void set_pwm_period(int pin, uint8_t period);
  uint8_t get_pwm_period(int pin);
  void set_digital(int pin, bool level);
  bool get_digital(int pin);
  void set_analog(int pin, int value);
  int get_analog(int pin);
  std::array<int, NUM_LEDS> get_all_leds();
  void set_led(int led, uint8_t brightness);
  void increment_counter(uint32_t us);
  uint64_t get_micros();
};


#endif
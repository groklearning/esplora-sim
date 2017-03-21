#include <iostream>
#include <cstdio>
#include <string>
#include <cstring>
#include "Arduino.h"
#include "Hardware.h"
#include "Serial.h"
#include "WString.h"
#include "Print.h"

void serial::begin(unsigned long baud_rate)
{
  // digitalMode[0] = RX;
  // digitalMode[1] = TX;
  for (int i = 0; i < 12; i++) {
    if (baud_rate == _possible_bauds[i]) {
      _baud_rate = baud_rate;
      return;
    }
  }
  _baud_rate = 9600;
}


void serial::end()
{
  // digitalMode[0] = FREE;
  // digitalMode[1] = FREE;

}

int serial::available()  // returns the number of bytes available to read
{
//  increment_counter(12);
  return (1);
}

char serial::read() // the first byte of incoming serial data available (or -1 if no data is available)
{
//  increment_counter(100);
  return (-1);
}

int serial::peek()
{
//  increment_counter(100);
  return (-1);
}

void serial::flush()
{
}

void serial::print(int x)
{
//  increment_counter(100);
  std::cout << x;

}

void print_binary(int number)
{
  if (number) {
    print_binary(number >> 1);
    putc((number & 1) ? '1' : '0', stdout);
  }
}

void serial::print(int x, int base)
{
//  increment_counter(16);
  char buf[20];
  switch (base) {
  case BIN:
    print_binary(x);
    break;
  case OCT:
    sprintf(buf, "%o", x);
    std::cout << buf;
    break;
  case DEC:
    sprintf(buf, "%d", x);
    std::cout << buf;
    break;
  case HEX:
    sprintf(buf, "%x", x);
    std::cout << buf;
    break;
  }
}

void serial::print(const String &s)
{
  int len = s.length();
  for (unsigned i = 0; i < s.length(); i++) {
    putchar(s[i]);
  }
  increment_counter(28 + (14 * len));
}


void serial::print(const char *p)
{
  std::cout << p;
  int len = strlen(p);
  // measured on Arduino esplora 9600 baud
  increment_counter(28 + (14 * len));
}

void serial::print(unsigned char uc)
{
  increment_counter(28);
  std::cout << uc;

}


void serial::println(int x)
{
  increment_counter(28);
  std::cout << x << std::endl;

}

void serial::println(int x, int base)
{
  print(x, base);
  std::cout << std::endl;

}

void serial::println(const char *p)
{
  increment_counter(28);
  std::cout << p << std::endl;

}

void serial::println(std::string s)
{
  increment_counter(28);
  const char *p;
  p = s.c_str();
  std::cout << p << std::endl;
}

void serial::println(const String &s)
{
  int len = s.length();
  print(s);
  println();
  increment_counter(28 + (14 * len));
}
void serial::println()
{
  increment_counter(28); // measured on Esplora
  std::cout << std::endl;
}

void serial::println(unsigned char uc)
{
  increment_counter(65);
  std::cout << uc << std::endl;
}

void serial::write(char *p)
{
  print(p);
}

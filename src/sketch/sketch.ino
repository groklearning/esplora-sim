#include <Esplora.h>
void setup() {}
int count = 0;

void loop() {
  if (Esplora.readButton(1) == LOW) {
    Esplora.writeRed(127);
  }
  else {
    Esplora.writeRGB(0, 0, 0);
  }
  int x = my_func();
  Serial.println(x);
  delay(2000);

}

int my_func () {
  return 4;
}


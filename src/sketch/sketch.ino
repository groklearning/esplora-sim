#include <Esplora.h>
void setup() {
  pinMode(13, OUTPUT);
}
int count = 0;

void loop() {
  digitalWrite(13, HIGH);
  delay(1000);
  digitalWrite(13, LOW);
  delay(1000);
  if (Esplora.readButton(1) == LOW) {
    Esplora.writeRed(127);
  }
  else {
    Esplora.writeRGB(0, 0, 0);
  }

}

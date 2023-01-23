#include <SPI.h>


void setup() {
  // put your setup code here, to run once:
  SPI.begin();
  SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
}

void loop() {
  // put your main code here, to run repeatedly:

}

#include <SPI.h>

const uint8_t chipSelect = 10;
const uint8_t MOTION_REG = 0x02;
const uint8_t DELTA_X_L = 0x03;
const uint8_t DELTA_X_H = 0x04;
const uint8_t DELTA_Y_L = 0x05;
const uint8_t DELTA_Y_H = 0x06;
const uint8_t CONFIG3 = 0x23;
const uint8_t PWR_UP_RST = 0x3A;
const uint8_t READ = 0b00000000; // READ command with 0 on MSB
const uint8_t WRITE = 0b10000000; // WRITE command with 1 on MSB

void setup() {
  // Set chipSelect as an output
  pinMode(chipSelect, OUTPUT);
  
  // Initialize the SPI communication
  SPI.begin();
  
  // Set the clock speed for the SPI communication
  SPI.setClockDivider(SPI_CLOCK_DIV2);

  pwrUp();

}

void loop() {
  // Check if motion has occurred
  if (readMotion()) {
    // Read the accumulated motion
    int deltaX = readDeltaX();
    int deltaY = readDeltaY();
    
    // Print the accumulated motion
    Serial.print("Delta X: ");
    Serial.println(deltaX);
    Serial.print("Delta Y: ");
    Serial.println(deltaY);
  }
}

bool readMotion() {
  // Take the chipSelect pin low to select the device
  digitalWrite(chipSelect, LOW);
    
  // Read the motion register
  uint8_t motion = readRegister(MOTION_REG, 1);
  
  // Take the chipSelect pin high to deselect the device
  digitalWrite(chipSelect, HIGH);
  
  return motion & 0x80;
  // Check if motion has occurred
  // if (motion & 0x80) {
  //   return true;
  // }
  // else {
  //   return false;
  // }
}

int readDeltaX() {
  // Take the chipSelect pin low to select the device
  digitalWrite(chipSelect, LOW);
  uint8_t deltaX_L = readRegister(DELTA_X_L, 1);
  digitalWrite(chipSelect, HIGH);
  delay(5);
  digitalWrite(chipSelect, LOW);
  uint8_t deltaX_H = readRegister(DELTA_X_H, 1);
  digitalWrite(chipSelect, HIGH);

  int deltaX = (deltaX_H << 8) | deltaX_L;
  
  return deltaX;
}

int readDeltaY() {
  // Take the chipSelect pin low to select the device
  digitalWrite(chipSelect, LOW);
  uint8_t deltaY_L = readRegister(DELTA_Y_L, 1);
  digitalWrite(chipSelect, HIGH);
  delay(5);
  digitalWrite(chipSelect, LOW);
  uint8_t deltaY_H = readRegister(DELTA_Y_H, 1);
  digitalWrite(chipSelect, HIGH);

  int deltaX = (deltaY_H << 8) | deltaY_L;
  
  return deltaX;
}

void pwrUp(){ // Power up sequence, see datasheet
  delay(8); 
  digitalWrite(chipSelect, HIGH);
  digitalWrite(chipSelect, LOW);
  writeRegister(PWR_UP_RST, 0x5A); 
  delay(5);
  writeRegister(CONFIG3, 0x01);
  // delay(1); 
  // writeRegister(0x25, 0x04);
  // writeRegister(0x13, 0x1D);
  // delay(10)
  // writeRegister(0x13, 0x18);
  delay(35);
  readRegister(0x02, 1);
  readRegister(0x03, 1);
  readRegister(0x04, 1);
  readRegister(0x05, 1);
  readRegister(0x06, 1);
  delay(50);
  digitalWrite(chipSelect, HIGH);
}


void writeRegister(uint8_t thisRegister, uint8_t thisValue) {

  uint8_t dataToSend = thisRegister | WRITE;

  // take the chip select low to select the device:

  digitalWrite(chipSelect, LOW);

  SPI.transfer(dataToSend); //Send register location

  SPI.transfer(thisValue);  //Send value to record into register

  // take the chip select high to de-select:

  digitalWrite(chipSelect, HIGH);
}

unsigned int readRegister(uint8_t thisRegister, int bytesToRead) {

  uint8_t inByte = 0;           // incoming byte from the SPI

  unsigned int result = 0;   // result to return

  Serial.print(thisRegister, BIN);

  Serial.print("\t");

  uint8_t dataToSend = thisRegister | READ;

  Serial.println(thisRegister, BIN);

  // take the chip select low to select the device:

  digitalWrite(chipSelect, LOW);

  // send the device the register you want to read:

  SPI.transfer(dataToSend);

  // send a value of 0 to read the first byte returned:

  result = SPI.transfer(0x00);

  // decrement the number of bytes left to read:

  bytesToRead--;

  // if you still have another byte to read:

  if (bytesToRead > 0) {

    // shift the first byte left, then get the second byte:

    result = result << 8;

    inByte = SPI.transfer(0x00);

    // combine the byte you just got with the previous one:

    result = result | inByte;

    // decrement the number of bytes left to read:

    bytesToRead--;

  }

  // take the chip select high to de-select:

  digitalWrite(chipSelect, HIGH);

  // return the result:

  return (result);
}


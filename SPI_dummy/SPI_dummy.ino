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
  
  // Send the address of the motion register
  SPI.transfer(MOTION_REG);
  
  // Read the motion register
  byte motion = SPI.transfer(0x00);
  
  // Take the chipSelect pin high to deselect the device
  digitalWrite(chipSelect, HIGH);
  
  // Check if motion has occurred
  if (motion & 0x01) {
    return true;
  }
  else {
    return false;
  }
}

int readDeltaX() {
  // Take the chipSelect pin low to select the device
  digitalWrite(chipSelect, LOW);
  
  // Send the address of the Delta_X_L register
  SPI.transfer(DELTA_X_L);
  
  // Read the Delta_X_L and Delta_X_H registers
  byte deltaX_L = SPI.transfer(0x00);
  byte deltaX_H = SPI.transfer(0x00);
  
  // Take the chipSelect pin high to deselect the device
  digitalWrite(chipSelect, HIGH);
  
  // Combine the two bytes to form a 16-bit value
  int deltaX = (deltaX_H << 8) | deltaX_L;
  
  return deltaX;
}

int readDeltaY() {
  // Take the chipSelect pin low to select the device
  digitalWrite(chipSelect, LOW);
  
  // Send the address of the Delta_Y_L register
  SPI.transfer(DELTA_Y_L);
  
  // Read the Delta_Y_L and Delta_Y_H registers
  byte deltaY_L = SPI.transfer(0x00);
  byte deltaY_H = SPI.transfer(0x00);
  
  // Take the chipSelect pin high to deselect the device
  digitalWrite(chipSelect, HIGH);
  
  // Combine the two bytes to form a 16-bit value
  int deltaY = (deltaY_H << 8);
}

void pwrUp(){
  delay(8); // wait 8 ms after the power applied to VDD & VDDIO

  digitalWrite(chipSelect, HIGH);
  digitalWrite(chipSelect, LOW); // HIGH then LOW on NSC to reset the SPI port

  writeRegister(PWR_UP_RST, 0x5A); // Write to Power_Up_Reset with command 0x5A

  delay(5) // wait 5 ms after writing to Power_Up_Reset

  writeRegister(CONFIG3, 0x01) // Write to Config3 with command 0x01

  delay(1) // Wait 1ms before SROM download

  writeRegister()


}


void writeRegister(byte thisRegister, byte thisValue) {

  byte dataToSend = thisRegister | WRITE;

  // take the chip select low to select the device:

  digitalWrite(chipSelect, LOW);

  SPI.transfer(dataToSend); //Send register location

  SPI.transfer(thisValue);  //Send value to record into register

  // take the chip select high to de-select:

  digitalWrite(chipSelect, HIGH);
}

unsigned int readRegister(byte thisRegister, int bytesToRead) {

  byte inByte = 0;           // incoming byte from the SPI

  unsigned int result = 0;   // result to return

  Serial.print(thisRegister, BIN);

  Serial.print("\t");

  byte dataToSend = thisRegister | READ;

  Serial.println(thisRegister, BIN);

  // take the chip select low to select the device:

  digitalWrite(chipSelectPin, LOW);

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

  digitalWrite(chipSelectPin, HIGH);

  // return the result:

  return (result);
}


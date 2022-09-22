/***********************************************************************/
/* Copyright (c) 2022 Clay Kress                                       */
/*                                                                     */
/* This file is part of AzulHW.                                        */
/* AzulHW is free software: you can redistribute it and/or modify it   */
/* under the terms of the GNU General Public License as published by   */
/* the Free Software Foundation, either version 3 of the License, or   */
/* (at your option) any later version.                                 */
/*                                                                     */
/* AzulHW is distributed in the hope that it will be useful,           */
/* but WITHOUT ANY WARRANTY; without even the implied warranty         */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.             */
/*                                                                     */
/* See the GNU General Public License for more details.                */
/*                                                                     */
/* You should have received a copy of the GNU General Public License   */
/* along with AzulHW. If not, see <https://www.gnu.org/licenses/>.     */
/*                                                                     */
/***********************************************************************/

void blnk() {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(100);                        // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(100);
}

class EEPROMProgrammer {
  private:
    const byte SHIFT_DATA= 2;
    const byte SHIFT_CLOCK= 3;
    const byte SHIFT_LATCH= 4;
    const byte SHIFT_CLEAR= 5;

    const byte ROM_WRITE= 6;
    const byte ROM_READ= 7;

    const int dataPins[8]= {13, 14, 15, 8, 9, 10, 11, 12};
    word chipLen;

  private:
    void romWrite() {
      digitalWrite(ROM_WRITE, LOW);
      delayMicroseconds(2);
      digitalWrite(ROM_WRITE, HIGH);
      delay(5);
      return;
    }

    void outputEnable() {
      digitalWrite(ROM_READ, LOW);
      return;
    }

    void outputDisable() {
      digitalWrite(ROM_READ, HIGH);
      return;
    }

    void pulseLatch() {
      digitalWrite(SHIFT_LATCH, HIGH);
      digitalWrite(SHIFT_LATCH, LOW);
      return;
    }

    void shiftClear() {
      digitalWrite(SHIFT_CLEAR, LOW);
      pulseLatch();
      digitalWrite(SHIFT_CLEAR, HIGH);
      return;
    }

    void shiftWrite(word value) {
      shiftClear();
      shiftOut(SHIFT_DATA, SHIFT_CLOCK, MSBFIRST, value >> 8);    // MSB
      shiftOut(SHIFT_DATA, SHIFT_CLOCK, MSBFIRST, value & 0xff);  // LSB
      pulseLatch();
      return;
    }

  public:
    EEPROMProgrammer(word pins) {

      this->chipLen= round(pow(2, pins));

      // Set shift register pins
      digitalWrite(SHIFT_DATA, LOW);    // Active High
      pinMode(SHIFT_DATA, OUTPUT);

      digitalWrite(SHIFT_CLOCK, LOW);   // Active High
      pinMode(SHIFT_CLOCK, OUTPUT);

      digitalWrite(SHIFT_LATCH, LOW);   // Active High
      pinMode(SHIFT_LATCH, OUTPUT);

      digitalWrite(SHIFT_CLEAR, HIGH);  // Active Low
      pinMode(SHIFT_CLEAR, OUTPUT);

      // Setup the ROM data pins
      for (int i= 0; i < 8; i++) {
        pinMode(dataPins[i], INPUT);
      }

      // Set ROM control pins
      digitalWrite(ROM_WRITE, HIGH);    // Active Low
      pinMode(ROM_WRITE, OUTPUT);

      digitalWrite(ROM_READ, HIGH);     // Active Low
      pinMode(ROM_READ, OUTPUT);
    }

    byte readByte(word address) {
      // Set the address
      shiftWrite(address);

      // Setup the ROM data pins
      for (int i= 0; i < 8; i++) {
        pinMode(dataPins[i], INPUT);
      }

      outputEnable(); // Enable ROM output

      // Read the data pins
      byte value= 0;
      for (int i= 0; i < 8; i++) {
        value |= digitalRead(dataPins[i]) ? round(pow(2, i)) : 0;
      }

      outputDisable(); // Disable ROM output
      return value;
    }

    void writeByte(byte value, word address) {
      // Set the address
      shiftWrite(address);
      outputDisable();

      // Setup the ROM data pins
      for (int i= 0; i < 8; i++) {
        pinMode(dataPins[i], OUTPUT);
      }

      // Set the data pins
      for (int i= 0; i < 8; i++) {
        digitalWrite(dataPins[i], value & 1);
        value= value >> 1;
      }

      // Toggle write enable pin
      romWrite();

      return;
    }
};

EEPROMProgrammer* chip;

void newData();
byte getByte();

void setup() {
  // Enable Serial
  Serial.begin(9600);
  Serial.print("<Programmer Ready>");
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  if (Serial.available()) {
    newData();
  }
}

void newData() {
  byte newCmd= Serial.read();
  word address= 0;
  byte data= 0;
  
  switch (newCmd) {
    case (0): // new
      chip= new EEPROMProgrammer(getByte());
      break;
      
    case (1): // del
      delete chip;
      break;
      
    case (2): // read
      address= 0;
      data= 0;
      address += getByte() * 256; // Get high byte first
      address += getByte();       // Get low byte next
      data= chip->readByte(address);
      Serial.print(data, DEC);
      break;
      
    case (3): // write
      address= 0;
      data= 0;
      address += getByte() * 256; // Get high byte first
      address += getByte();       // Get low byte next
      data += getByte();          // Get data byte last
      chip->writeByte(data, address);
      Serial.print(1);
      break;
      
    default:
      break;
  }
}

byte getByte() {
  // Wait for next byte
  while(!Serial.available()) {
    continue;
  }
  byte newByte= Serial.read();
  return newByte;
}

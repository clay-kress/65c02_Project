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

const char CLK= 2;
const char RWB= 3;

//const char ADDR[]= {22,24,26,28,30,32,34,36,38,40,42,44,46,48,50,52};
//const char DATA[]= {23,25,27,29,31,33,35,37};
//const char PRTB[]= {53,51,49,47,45,43,41,39};

const char ADDR[]= {52,50,48,46,44,42,40,38,36,34,32,30,28,26,24,22};
const char DATA[]= {37,35,33,31,29,27,25,23};
const char PRTB[]= {39,41,43,45,47,49,51,53};
const char RS= 4;
const char RW= 5;
const char E= 6;

int counter;

word readAddr();

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);

  counter= 0;

  pinMode(CLK, OUTPUT);
  pinMode(RWB, INPUT);
  for (int i= 0; i < 16; i++) {
    pinMode(ADDR[i], INPUT);
  }
  for (int i= 0; i < 8; i++) {
    pinMode(DATA[i], INPUT);
  }
  for (int i= 0; i < 8; i++) {
    pinMode(PRTB[i], INPUT);
  }
  pinMode(RS, INPUT);
  pinMode(RW, INPUT);
  pinMode(E, INPUT);
}

// the loop function runs over and over again forever
void loop() {
  char output[20];
  
  unsigned int address= 0;
  for (int i= 0; i < 16; i++) {
    int bit= digitalRead(ADDR[i]) ? 1 : 0;
    Serial.print(bit);
    address= (address << 1) + bit;
  }
  
  Serial.print("   ");

  unsigned int data= 0;
  for (int i= 0; i < 8; i++) {
    int bit= digitalRead(DATA[i]) ? 1 : 0;
    Serial.print(bit);
    data= (data << 1) + bit;
  }

  unsigned int portb= 0;
  for (int i= 0; i < 8; i++) {
    int bit= digitalRead(PRTB[i]) ? 1 : 0;
    portb= (portb << 1) + bit;
  }

  unsigned int rs= digitalRead(RS) ? 1 : 0;
  unsigned int rw= digitalRead(RW) ? 1 : 0;
  unsigned int e= digitalRead(E) ? 1 : 0;

  sprintf(output, "   %04x %c %02x, %01x %01x %01x %02x", address, digitalRead(RWB) ? 'r' : 'W', data, rs, rw, e, portb);
  Serial.println(output);
  digitalWrite(CLK, HIGH);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(20);
  digitalWrite(CLK, LOW);
  digitalWrite(LED_BUILTIN, LOW);
  delay(20);
}

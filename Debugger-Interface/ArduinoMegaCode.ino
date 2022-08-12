const char CLK= 2;
const char RWB= 3;

//const char ADDR[]= {22,24,26,28,30,32,34,36,38,40,42,44,46,48,50,52};
//const char DATA[]= {23,25,27,29,31,33,35,37};

const char ADDR[]= {52,50,48,46,44,42,40,38,36,34,32,30,28,26,24,22};
const char DATA[]= {37,35,33,31,29,27,25,23};

int counter;

word readAddr();

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);

  counter= 0;

  pinMode(CLK, INPUT);
  pinMode(RWB, INPUT);
  for (int i= 0; i < 16; i++) {
    pinMode(ADDR[i], INPUT);
  }
  for (int i= 0; i < 8; i++) {
    pinMode(DATA[i], INPUT);
  }
  attachInterrupt(digitalPinToInterrupt(CLK), onClock, RISING);
}

void onClock() {
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

  sprintf(output, "   %04x %c %02x", address, digitalRead(RWB) ? 'r' : 'W', data);
  Serial.println(output);
}

// the loop function runs over and over again forever
void loop() {
}
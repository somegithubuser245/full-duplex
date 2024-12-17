void setup() {
  DDRD = 0x00;
  DDRB = 0xFF;
  
  Serial.begin(9600);
}

void loop() {
  PORTB = (PIND >> 4);
}

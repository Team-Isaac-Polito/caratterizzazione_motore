// CONFIGURAZIONE PIN
#define PIN_EN    4
#define PIN_IN1   5
#define PIN_IN2   6
#define PIN_PWM   7
#define PIN_ENC_A 2
#define PIN_ENC_B 3 

int count;

void setup() {
  int rpm,tempo;

  // SERIALE
  Serial.begin(115200);
  Serial.println("Format: (time in milliseconds,direction of the step)");


  // DRIVER MOTORI
  //enable pin
  pinMode(PIN_EN, OUTPUT);

  //motore1
  pinMode(PIN_IN1, OUTPUT);
  pinMode(PIN_IN2, OUTPUT);
  pinMode(PIN_PWM, OUTPUT);
 
  digitalWrite(PIN_EN, HIGH);     //attivo il driver - enable
  analogWrite(PIN_PWM, 0);        // motore fermo

  // ENCODER
  pinMode(PIN_ENC_A,INPUT);
  pinMode(PIN_ENC_B,INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_ENC_A), handleEncoder, RISING);
  
  tempo = millis();
  while(millis() < tempo + 300) printData();

  analogWrite(PIN_PWM, 255);        // avvio il motore

  for(int i = 0; i < 2; i++) {
    // avanti
    digitalWrite(PIN_IN1,HIGH);
    digitalWrite(PIN_IN2,LOW);

    // aspetto un secondo
    tempo = millis();
    while(millis() < tempo + 1000) printData();

    // indietro
    digitalWrite(PIN_IN1,LOW);
    digitalWrite(PIN_IN2,HIGH);

    // aspetto un secondo
    tempo = millis();
    while(millis() < tempo + 1000) printData();
  }
  analogWrite(PIN_PWM,0);

  // aspetto un secondo
  tempo = millis();
  while(millis() < tempo + 1000) printData();
}

void loop(){}

//ISR dell'interrupt motore R, viene richiamata ad ogni fronte di salita del segnale nell' encoder1A (RISING)
void handleEncoder()
{
      count += digitalRead(PIN_ENC_B) == HIGH ? 1 : -1; // aggiungo o tolgo un passo in base al segnale del pin direzione
}

void getEncoderData(int &rpmL,int &prevMillis)
{
    ////calcolo numero di giri albero////
    cli();                                                              // AG INIZIO operazione atomica - non può essere interrotta da interrupt
    int millisec = millis();
    rpmL = ((float(count) / 12.0) * (60.0 * 1000.0 /(millisec - prevMillis))) / 74.83;
    prevMillis = millisec;

    ///* quando count=12 il motore effettua un giro (interno, non dell'albero)[trasformo gli impulsi in giri]
    //(60.0 * 1000 / DT) passo da ms a minuti
    ///74.83) divido per il rapporto di trasmissione del motore

    count = 0;
    sei();                                                              // AG FINE operazione atomica - non può essere interrotta da interrupt
}
void printData() {
  static int rpm;
  static int previousMillis;
  getEncoderData(rpm,previousMillis);
  Serial.print(rpm);
  Serial.print(",");
  Serial.println(previousMillis);
}

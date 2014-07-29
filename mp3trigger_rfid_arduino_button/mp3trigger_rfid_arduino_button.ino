#include <SoftwareSerial.h>
#include <MP3Trigger.h>

SoftwareSerial RFID(2,3); // define port for Port1
SoftwareSerial trigSerial = SoftwareSerial(4, 5); // define port for MP3Trigger
//SoftwareSerial RFIDSec(6,7); //define digital pin for Port2

MP3Trigger trigger;


char code[13];
int current_card = -1;
int number_of_cards = 5;
char *card[]      = {"30008C2D76E7",  "4B00DD81B0A7"     , "30008BDE1D78", "", "310022DFFC30"}; // uma matriz onde cada linha e um cartao
char *card_name[] = {"keychain"    , "keychainSkyScanner",  "blanktag"   ,     "Birmingham",   "McGranran"};
int song[] = {2, 3, 1};
int song_delay[] = {2000, 2000, 2000};

int j;
boolean done = false; // boolean fica verdadeiro depois que ele terminar a leitura de um cartao

const int rfidLight = 13; // pin for LED
const int buttonPin = 11; // pin for button


void setup()
{
  Serial.begin(9600);
  
  // Start serial communication with the trigger (over SoftwareSerial)
  trigger.setup(&trigSerial);
  
  RFID.begin(9600); //search for Port1 for RFID
  //  RFIDSec(9600);    // serach for Port2 for RFID
  trigSerial.begin(MP3Trigger::serialRate());
  
  pinMode(buttonPin, INPUT);
  pinMode(rfidLight, OUTPUT);
 
}

void readCard()
{
  int i=0;
  while(RFID.available()>0) //Enquanto tiver cartao para ler
  {
    char c = RFID.read();   //Leia um byte
    if (i >= 1 && i <= 12)  //Se for entre 1 e 12, e um dos bytes que fazem parte do ID certo 
    {
      code[i-1] = c;       //Atribua na nossa variavel de cartao -- -1 pois o vetor comeca a contar em 0 e nao em 1
    }
    if (i == 13)
      done = true;          // significa que a variavel code tem o valor do ultimo cartao lido
    i++;
    delay(100);             //Espereo 100 ms para entrar outro byte no buffer de leitura
  }
}


void lightLED()
{
  digitalWrite(rfidLight, HIGH);     // acender e apagar led quando detectar cartao
  delay(1000);
  
}

void checkButtonAndPlay()
{
  int state = HIGH;      // the current state of the output pin
  int reading;           // the current reading from the input pin
  int previous = LOW;    // the previous reading from the input pin
  long time = 0;         // the last time the output pin was toggled
  long debounce = 200;   // the debounce time, increase if the output flickers
  
  reading = digitalRead(buttonPin);
  if (reading == HIGH && previous == LOW && millis() - time > debounce)
  {
    if (state == HIGH)
    {
          trigSerial.listen();
          //trigger.update();
          //setVolume.trigger(10);
          trigger.trigger(song[current_card]);
          delay(song_delay[current_card]);
          trigger.stop();
          digitalWrite(rfidLight, LOW);
    }
    else if (state == LOW)
    {
      state = HIGH;
    }
    time  = millis();
  }
  previous = reading;
}

void loop()
{
  int i;
  RFID.listen();
  readCard();
  if (done)
  {
    for (j=0; j< number_of_cards; j++) // j contador de linhas
    {
      if (strcmp(card[j], code) == 0) //string compare caracteres - 0 means it is equal
      {
        Serial.print("Card read! ");
        Serial.println(card_name[j]);
        lightLED();
        current_card = j;
        break;
      }
    }
    done = false; // parar quando o if de cima for verdadeiro
  }
  if (current_card != -1) // se code for diferente de 0, que foi inicializado
  {
    checkButtonAndPlay();
    
  }
}

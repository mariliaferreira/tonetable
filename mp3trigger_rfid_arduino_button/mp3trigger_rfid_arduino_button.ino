#include <SoftwareSerial.h>
#include <MP3Trigger.h>


// ===================GLOBAL VARIABLES=====================
// RFID AND mp3 trigger
SoftwareSerial RFID(2,3); // define port for Port1
SoftwareSerial trigSerial = SoftwareSerial(4, 5); // define port for MP3Trigger
MP3Trigger trigger;
//SoftwareSerial RFIDSec(6,7); //define digital pin for Port2


// Card variables
char code[13];            //Stores string of the current card code
int current_card = -1;    //Index of the current card
int number_of_cards = 5;  //Integer containing the number of cards
// String matrix. Each line is a char array containg the card code.
char *card[]      = {"30008C2D76E7",  "4B00DD81B0A7"     , "30008BDE1D78", "", "310022DFFC30"}; 
// String matrix. Each line is a char array containg the card name.
char *card_name[] = {"keychain"    , "keychainSkyScanner",  "blanktag"   ,     "Birmingham",   "McGranran"};
int song[] = {2, 3, 1};   // Integers containing song identifiers
int song_delay[] = {2000, 2000, 2000}; // Integers containing song delays


// Led variables
int current_led = 0;    //Index of the current led to be lid
int number_of_leds = 5; //Integer containing the number of leds
int led_pins[] = {19, 20, 21, 22, 26}; //Pin number of each of led. They should be in the order that you want them to lid


// Pins variables
const int rfidLight = 13;      // pin for LED
const int soundButtonPin = 11; // pin for sound button
const int ledButtonPin = 12;   // pin for led blink button


int j;
boolean done = false; // True after reading the 12 digits of a card string.


// ===================Initialization=====================
void setup()
{
  Serial.begin(9600);
  
  // Start serial communication with the trigger (over SoftwareSerial)
  trigger.setup(&trigSerial);
  
  RFID.begin(9600); //search for Port1 for RFID
  //  RFIDSec(9600);    // serach for Port2 for RFID
  trigSerial.begin(MP3Trigger::serialRate());
  
  pinMode(soundButtonPin, INPUT); //Defining the sound button as input
  pinMode(ledButtonPin, INPUT);   //Defining the led button as input
  pinMode(rfidLight, OUTPUT);

  //Defining all the leds as output. 
  for (j=0; j<number_of_leds; j++)
    pinMode(led_pins[j], OUTPUT);
 
}

void readCard()
{
  int i=0;
  while(RFID.available()>0) //While there is information to read
  {
    char c = RFID.read();   //Read a byte
    if (i >= 1 && i <= 12)  //Only bytes between 1 and 12 concerns us
    {
      code[i-1] = c;       //Set on code array at index-1. An array starts on 0.
    }
    if (i == 13)
      done = true;         //Means that all digits were read.
    i++;
    delay(100);            //Waits for 100 ms until another byte arrives at the input buffer
  }
}


void lightLED()
{
  digitalWrite(rfidLight, HIGH); //Turn On the led when a card is read
  delay(1000);
  
}

//@buttonPin: Integer. Contains the pin number of the button that will be checked.
void checkButtonPress(buttonPin)
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
      // ===================Sound Action=====================
      if (buttonPin == soundButtonPin) //If the button checked is the sound button
      {                                //Play the sounds
        trigSerial.listen();
        //trigger.update();
        //setVolume.trigger(10);
        trigger.trigger(song[current_card]);
        delay(song_delay[current_card]);
        trigger.stop();
        digitalWrite(rfidLight, LOW);
      }
      // ===================LED Action=====================
      else if (buttonPin == ledButtonPin) // If the button checked is the led button
      {                                   
        digitalWrite(led_pins[current_led], HIGH); // turn the current_led ON
        delay(1000);                               // wait for a second
        digitalWrite(led_pins[current_led], LOW);  // turn the current_led off
        delay(1000);   
        current_led++;  // increment the current_led. So in the next iteration it will lid the led with pin on led_pins[1]. The third iteration led_pins[2]....
        if (current_led == number_of_leds) //If it reaches the number_of_leds, it starts back on led pin at index 0 on the next time you press the button.
          current_led = 0;
      }
    }
    else if (state == LOW)
      state = HIGH;
    time  = millis();
  }
  previous = reading;
}

void loop()
{
  // ===================LED Button=====================
  //Nothing related to card reading.
  checkButtonPress(ledButtonPin); //Check if the led button was pressed. And light the led

  
  // ===================Card Reading=====================  
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
        current_card = j;  //current_card stores the current 
        break;
      }
    }
    done = false; // parar quando o if de cima for verdadeiro
  }

  // ===================Sound Button=====================
  if (current_card != -1) // If a card has been read.
    checkButtonPress(soundButtonPin); //Check the sound button and play.
}

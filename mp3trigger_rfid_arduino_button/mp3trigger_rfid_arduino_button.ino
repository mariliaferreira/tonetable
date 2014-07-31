#include <SoftwareSerial.h>
#include <MP3Trigger.h>


// ===================GLOBAL VARIABLES=====================
// RFID AND mp3 trigger
SoftwareSerial RFID(2,3); // define port for Port1
//SoftwareSerial RFIDSec(6,7); //define digital pin for Port2
SoftwareSerial trigSerial = SoftwareSerial(4, 5); // define port for MP3Trigger
MP3Trigger trigger;



// Card variables
char code[13];            //Stores string of the current card code
char codeSec[13];            //Stores string of the current card code second rfid
int current_card = -1;    //Index of the current card
int current_cardSec = -1;    //Index of the current card second rfid
int number_of_cards = 18;  //Integer containing the number of cards
// String matrix. Each line is a char array containg the card code.
char *card[]      = {"30008C332FA0",  "30008C0CF747" , "310022F37191", "30008C25148D", "30008C20D844", "30008C1AE741", "30008C3E67E5", "30008BE3DC84", "30008C1CD676", "30008BD97012", "31002300E7F5", "310022F3BE5E", "30008C3950D5", "30008BE9F9AB", "310022F28B6A", "310022E15EAC", "30008C0F4DFE", "30008C4137CA"}; 
// String matrix. Each line is a char array containg the card name.
char *card_name[] = {"a2"          , "b2"            , "c2"          , "d2"          , "e2"          , "f2"         , "g2"          , "h2"          , "i2"          , "j2"          , "k2"          , "l2"          , "m2"          , "n2"          , "o2"          , "p2"          , "q2"          , "r2"          };
int song[] = {2, 3, 1};   // Integers containing song identifiers
int song_delay[] = {2000, 2000, 2000}; // Integers containing song delays


// Led variables
int current_led = 1;    //Index of the current led to be lid
int number_of_leds = 5; //Integer containing the number of leds
int led_pins[] = {A0, A1, A2, A3, A4}; //Pin number of each of led. They should be in the order that you want them to lid


// Pins variables
const int rfidLight = 13;      // pin for LED rfid 1
//const int rfidLightSec = A5;     // pin for LED rfid 2
const int soundButtonPin = 11; // pin for sound button
//const int soundButtonPin2 = 12; // pin for sound button
const int ledButtonPin = 10;   // pin for led blink button


int j;
boolean done = false; // True after reading the 12 digits of a card string.


// ===================Initialization=====================
void setup()
{
  Serial.begin(9600);
  
  // Start serial communication with the trigger (over SoftwareSerial)
  trigger.setup(&trigSerial);
  
  RFID.begin(9600); //search for Port1 for RFID
  //RFIDSec.begin(9600); //search for Port2 for RFID
  trigSerial.begin(MP3Trigger::serialRate());
  
  pinMode(soundButtonPin, INPUT); //Defining the sound button as input
  //pinMode(soundButtonPin2, INPUT); //Defining the sound button2 as input
  pinMode(ledButtonPin, INPUT);   //Defining the button that controls LED panel
  pinMode(rfidLight, OUTPUT);
  //pinMode(rfidLightSec, OUTPUT);

  //Defining all the leds as output. 
  for (j=0; j<number_of_leds; j++)
  {
    pinMode(led_pins[j], OUTPUT);
    digitalWrite(led_pins[j], LOW);
  }
  digitalWrite(led_pins[0], HIGH); // let the first led ON
}
  

void readCardRFID1()
{
  int i=0;
  
  RFID.listen();
  
  while(RFID.available()>0) //While there is information to read
  {
    char c = RFID.read();   //Read a byte
    if (i >= 1 && i <= 12)  //Only bytes between 1 and 12 concerns us
    {
     code [i - 1] = c;      //Set on code array at index-1. An array starts on 0.
    }
    if (i == 13)
    {
      lightLED(rfidLight);
      //Serial.println(code);
      done = true;         //Means that all digits were read.
    }
    i++;
    delay(100);            //Waits for 100 ms until another byte arrives at the input buffer
  }
}

//void readCardRFID2()
//{
//  int i=0;
//  
//  RFIDSec.listen();
//  
//  while(RFIDSec.available()>0) //While there is information to read
//  {
//    char c = RFIDSec.read();   //Read a byte
//    if (i >= 1 && i <= 12)  //Only bytes between 1 and 12 concerns us
//    {
//     codeSec [i - 1] = c;      //Set on code array at index-1. An array starts on 0.
//    }
//    if (i == 13)
//    {
//      lightLED(rfidLightSec);
//      Serial.println(codeSec);
//      done = true;         //Means that all digits were read.
//    }
//    i++;
//    delay(100);            //Waits for 100 ms until another byte arrives at the input buffer
//  }
//}


void lightLED(const int led)
{
  digitalWrite(led, HIGH); //Turn On the led when a card is read
  delay(1000);
}

//@buttonPin: Integer. Contains the pin number of the button that will be checked.
void checkButtonPress(const int buttonPin)
{
  int state = HIGH;      // the current state of the output pin
  int reading;           // the current reading from the input pin
  int previous = LOW;    // the previous reading from the input pin
  long time = 0;         // the last time the output pin was toggled
  long debounce = 200;   // the debounce time, increase if the output flickers
  int previous_led;
  
  reading = digitalRead(buttonPin);
  if (reading == HIGH && previous == LOW && millis() - time > debounce)
  {
    if (state == HIGH)
    {
      //Serial.println(buttonPin);
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
//      else if (buttonPin == soundButtonPin2) //If press button 2
//      {
//        trigSerial.listen();
//        //trigger.update();
//        //setVolume.trigger(10);
//        trigger.trigger(song[current_cardSec]);
//        delay(song_delay[current_cardSec]);
//        trigger.stop();
//        digitalWrite(rfidLightSec, LOW);
//      }

      // ===================LED Action=====================
      else if (buttonPin == ledButtonPin) // if press button LED panel
      {                                   
        previous_led = current_led - 1;
        if(previous_led < 0){
          previous_led = number_of_leds - 1;
        }
        digitalWrite(led_pins[previous_led], LOW); // turn the current_led ON
        delay(200);
        digitalWrite(led_pins[current_led], HIGH);  // turn the current_led off 
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
  
  readCardRFID1();
  //readCardRFID2();
  
  if (done)
  {
    for (j=0; j< number_of_cards; j++) // j contador de linhas
    {
      if (strcmp(card[j], code) == 0) //string compare caracteres - 0 means it is equal
      {
        Serial.print("Card read first! ");
        Serial.println(card_name[j]);
        current_card = j;  //current_card stores the current 
        break;
      }
      
//      if (strcmp(card[j], codeSec) == 0) //string compare caracteres - 0 means it is equal
//      {
//        Serial.print("Card read second! ");
//        Serial.println(card_name[j]);
//        current_cardSec = j;  //current_card stores the current 
//        break;
//      }
      
    }
    done = false; // parar quando o if de cima for verdadeiro
  }

  // ===================Sound Button=====================
  if (current_card != -1) // If a card has been read.
    checkButtonPress(soundButtonPin); //Check the sound button and play.
    
//  if (current_cardSec != -1) // check if card was red on rfid 2
//    checkButtonPress(soundButtonPin2); //Check the sound button and play.
}

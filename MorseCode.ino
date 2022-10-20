/**
This program is designed to flash an led to transmit morse code.

The rules of signalling Morse Code messages are as follows:
1. Each dot is one unit, each dash is three units.
2. Leave one unit between parts of the same letter.
3. Leave three units between letters.
4. Leave seven units between words.
*/

#include <string.h>
#include <stdio.h>
#include <ctype.h>


uint8_t greenLed = 12;  // HERO board pin 12, LED
uint8_t buzzer = 11;  // HEO Board pin 11, buzzer
uint8_t transmitter = 10;  // HERO Board pin 10, AM transmitter
uint8_t redLed = 8;  // HERO board pin 10, used for debugging purposes

uint8_t sensorPin = A0;  // set A0 to input for probing the photoresistor
uint8_t sensorValue = 0;  // variable to store the value read from the sensor

uint8_t transAmRadio = 2;  // HERO board pin 2 , transmitt via radio on/off
uint8_t transAudibly = 3;  // HERO board pin 3, transmitt via sound on/off
uint8_t transVisually = 4;  // HERO board pin 4, transmitt visually on/off
  
int dit = 250;
int dah = dit * 3;
int letterBreak = dit * 3;
int wordBreak = dit * 7;
int partBreak = dit;

int maxSpeed = 250;  // this is as short as we ever want to see the dit value
int minSpeed = 1000;  // this is as long as we ever want to see the dit value
int sensorMax = 1024;  // the max value the sensor can return
int sensorMin = 0;  // the minimum value the sensor can return

// The first element of the array is the length of the actual data, sorta like string were in DOS
int letterCodes[26][5] = {
    {2, dit, dah},           // A
    {4, dah, dit, dit, dit}, // B
    {4, dah, dit, dah, dit}, // C
    {3, dah, dit, dit},      // D
    {4, dit, dit, dah, dit}, // F
    {1, dit},                // E
    {3, dah, dah, dit},      // G
    {4, dit, dit, dit, dit}, // H
    {2, dit, dit},           // I
    {4, dit, dah, dah, dah}, // J
    {3, dah, dit, dah},      // K
    {4, dit, dah, dit, dit}, // L
    {2, dah, dah},           // M
    {3, dah, dah, dah},      // O
    {2, dah, dit},           // N
    {4, dit, dah, dah, dit}, // P
    {4, dah, dah, dit, dah}, // Q
    {3, dit, dah, dit},      // R
    {3, dit, dit, dit},      // S
    {1, dah},                // T
    {3, dit, dit, dah},      // U
    {4, dit, dit, dit, dah}, // V
    {3, dit, dah, dah},      // W
    {4, dah, dit, dit, dah}, // X
    {4, dah, dit, dah, dah}, // Y
    {4, dah, dah, dit, dit}, // Z
};

int numberCodes[10][6] = {  
    {5, dah, dah, dah, dah, dah}, // '0'
    {5, dit, dah, dah, dah, dah}, // '1'
    {5, dit, dit, dah, dah, dah}, // '2'
    {5, dit, dit, dit, dah, dah}, // '3'
    {5, dit, dit, dit, dit, dah}, // '4'
    {5, dit, dit, dit, dit, dit}, // '5'
    {5, dah, dit, dit, dit, dit}, // '6'
    {5, dah, dah, dit, dit, dit}, // '7'
    {5, dah, dah, dah, dit, dit}, // '8'
    {5, dah, dah, dah, dah, dit}  // '9'
};

void sendLetter(int code[]);
void sendMessage(char message[], int);
bool checkSwitch(uint8_t);
void updateTransmittSpeed();

void setup() {
    
    Serial.begin(9600);

    // setup the output pins
    pinMode(transmitter, OUTPUT);
    pinMode(buzzer, OUTPUT);
  	pinMode(greenLed, OUTPUT);
    pinMode(redLed, OUTPUT);
}

void loop() {

    // the red LED is used to signal the start of the message
    digitalWrite(redLed, HIGH);  // signal start of message ( just so I know when it starts during the loop)
    delay(wordBreak);  // add in the pause between words
    digitalWrite(redLed, LOW);  // turn off the LED

    // update the transmitt speed based on the photoresistor

    char message[] = "I love you";
    sendMessage(message, strlen(message));
    delay(wordBreak);
}

void updateTransmittSpeed() {

    // get the current sensor value, currently this is a photoresistor
    sensorValue = analogRead(sensorPin); // values from 0 - 1024

    // calculate the multipler
    double multiplier = (float)(minSpeed - maxSpeed) / (sensorMax - sensorMin);

    // assign the values
    dit = minSpeed - (sensorValue * multiplier);
    double d_dit = sensorValue * multiplier;

    dah = dit * 3;
    partBreak = dit;
    letterBreak = dit * 3;
    wordBreak = dit * 7;

    Serial.print("Speeds - DIT: ");
    Serial.print(dit);
    Serial.print(" DAH: ");
    Serial.print(dah);
    Serial.print(" PART: ");
    Serial.print(partBreak);
    Serial.print(" LETTER: ");
    Serial.print(letterBreak);
    Serial.print(" WORD: ");
    Serial.print(wordBreak);
    Serial.print(" Multiplier ");
    Serial.print(multiplier);
    Serial.print(" Sensor: ");
    Serial.println(sensorValue);
    
}

void sendLetter(int code[]) {
  
    // get the state of the switches
    bool transVisually = checkSwitch(transVisually);
    bool transAudibly = checkSwitch(transAudibly);
    bool transAmRadio = checkSwitch(transAmRadio);

    updateTransmittSpeed();

    for (int i = 1; i <= code[0]; i++) {
        if (transVisually) {
            digitalWrite(greenLed, HIGH);  // Turn the Green LED on 
        }
        if (transAudibly) {
            digitalWrite(buzzer, HIGH);
        }
        if (transAmRadio) {
            digitalWrite(transmitter, HIGH);
        }

        // keep the LED on for the 
        delay(code[i]);
      
        digitalWrite(greenLed, LOW);  // no real need to check the transmitt status on turning them off
        digitalWrite(transmitter, LOW);
        digitalWrite(buzzer, LOW);	
      
        if (i < code[0]) {  // Check to see if there is more code left for this character
            delay(partBreak);  // add in the delay between the dits and dahs if this is not the last part of the code
        }
    }
}

/**
* 
*/
void sendMessage(char message[], int length) {

    // get the state of the switches
    bool transVisually = checkSwitch(transVisually);
    bool transAudiably = checkSwitch(transAudibly);
    bool transAmRadio = checkSwitch(transAmRadio);

    for (int i = 0; i < length; i++) {
        char letter = toupper(message[i]);
        if (letter >= 'A' && letter <= 'Z') {
            sendLetter(letterCodes[LetterOffset(letter)]);
        } else if (letter >= '0' && letter <= '9') {
            sendLetter(numberCodes[NumberOffset(letter)]);
        } else if (letter == ' ') {
            delay(wordBreak);
        }
        delay(letterBreak);  // add in the delay between leters of the word      
    }
}

/** 
* LetterOffset finds the offset into the letterCodes
* 
* A simple function that find the offset into the array by finding the integer distance
* between 'A" and the letter.
*/
int LetterOffset(char letter) {
    return int(letter) - int('A');
}

/** 
* NumberOffset finds the offset into the letterCodes
* 
* A simple function that find the offset into the array by finding the integer distance
* between '0' and the letter.
*/
int NumberOffset(char letter) {
    return int(letter) - int('0');
}

bool checkSwitch(uint8_t switchNum) {
    return digitalRead(switchNum) == HIGH;
}

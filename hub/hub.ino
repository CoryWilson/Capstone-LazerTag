/*
 Copyright (C) 2012 James Coliz, Jr. <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 
 Update 2014 - TMRh20
 */

/**
 * Simplest possible example of using RF24Network,
 *
 * RECEIVER NODE
 * Listens for messages from the transmitter and prints them out.
 */

#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include <IRremote.h>

RF24 radio(7,8);                // nRF24L01(+) radio attached using Getting Started board 

RF24Network network(radio);      // Network uses that radio
const uint16_t hub = 00;    // Address of our node in Octal format ( 04,031, etc)
const uint16_t player1 = 01;   // Address of the other node in Octal format
const uint16_t player2 = 02;   // Address of the third node in Octal format

struct payload_t {                 // Structure of our payload
  unsigned long ms;
  unsigned long counter;
  int buttonState;
  int playerId;
  bool hit;
};


void setup(void)
{
  Serial.begin(57600);
  Serial.println("RF24Network/examples/helloworld_rx/");
 
  SPI.begin();
  radio.begin();
  network.begin(/*channel*/ 90, /*node address*/ hub);
}

void loop(void){
  
  network.update();                  // Check the network regularly

  
  while ( network.available() ) {     // Is there anything ready for us?
    
    RF24NetworkHeader header;        // If so, grab it and print it out
    payload_t payload;
    network.read(header,&payload,sizeof(payload));
    Serial.print("Received packet #");
    Serial.print(payload.counter);
    Serial.print(" at ");
    Serial.println(payload.ms);
    Serial.print("button state ");
    Serial.println(payload.buttonState);
    Serial.print("Player ");
    Serial.println(payload.playerId);
    Serial.print("Hit ");
    Serial.println(payload.hit);
  }
}


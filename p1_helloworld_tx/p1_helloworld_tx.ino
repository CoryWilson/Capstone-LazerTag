/*
 Copyright (C) 2012 James Coliz, Jr. <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 
 Update 2014 - TMRh20
 */

/**
 * Simplest possible example of using RF24Network 
 *
 * TRANSMITTER NODE
 * Every 2 seconds, send a payload to the receiver node.
 */

#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>

RF24 radio(7,8);                    // nRF24L01(+) radio attached using Getting Started board 

RF24Network network(radio);          // Network uses that radio

uint16_t player;    // Address of our node in Octal format
//const uint16_t player1;    // Address of the third node in Octal format
const uint16_t hub = 00;        // Address of the other node in Octal format
      
const int buttonPin = 4;

int buttonState = 0;

const unsigned long interval = 500; //ms  // How often to send 'hello world to the other unit
int playerId;
unsigned long last_sent;             // When did we last send?
unsigned long packets_sent;          // How many have we sent already


struct payload_t {      // Structure of our payload
  //buttonState;
  unsigned long ms;
  unsigned long counter;
  int buttonState;
  int playerId;
};

void setup(void)
{
  Serial.begin(57600);
  Serial.println("RF24Network/examples/helloworld_tx/");
  
  pinMode(2,INPUT_PULLUP);
  pinMode(4,INPUT);
  
  if(digitalRead(2) == LOW){
    playerId = 1;  
    player = 01;
  } else {
    playerId = 2;
    player = 02;
  }
  
  SPI.begin();
  radio.begin();
  network.begin(/*channel*/ 90, /*node address*/ player);
}

void loop() {
  
  buttonState = digitalRead(buttonPin);
  
  network.update();                          // Check the network regularly  
  unsigned long now = millis();              // If it's time to send a message, send it!
  if ( now - last_sent >= interval  )
  {
    last_sent = now;

    Serial.print("Sending...");
    payload_t payload = { millis(), packets_sent++ ,buttonState, playerId};
    RF24NetworkHeader header(/*to node*/ hub);
    bool ok = network.write(header,&payload,sizeof(payload));
    if (ok)
      Serial.println("ok.");
    else 
      Serial.println("failed.");
    
  }
}



//INVOLT ARDUINO SKETCH
/*
  AUTO PINMODE
  Involt by default automatically adds pinMode to received
  pin data so you don't need to add pinMode output in setup for
  basic interactions. This mode is not recommended when
  mixing digital inputs and outputs.
*/
boolean autoPinMode = true;

/*
  DIRECT MODE
  Direct mode sends data from app to digital pins without storing
  it in chromeDigital array and without additional code in void
  loop. It can be used for testing interaction with Arduino
  inside App. In direct mode digitalWrite/analogWrite is
  automatic only for values received from Involt.
*/
boolean directMode = false;

/*
  Array for digital pins.
  IMPORTANT: You need to declare the total number of variables.
  Default is Arduino UNO total pin number +1.
*/
#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
int chromeDigital[14] = {};
String fname;

RF24 radio(9,10);                // nRF24L01(+) radio attached using Getting Started board 

RF24Network network(radio);      // Network uses that radio
const uint16_t hub = 00;    // Address of our node in Octal format ( 04,031, etc)
const uint16_t player1 = 01;   // Address of the other node in Octal format
const uint16_t player2 = 02;   // Address of the third node in Octal format

int p1_health = 100;
int p2_health = 100;
int damage = 10;

struct payload_t {                 // Structure of our payload
  unsigned long ms;
  unsigned long counter;
  int playerId;
  bool hit;
};

void setup() {
  //Do not change the serial connection bitrate.
  Serial.begin(115200);
  
  Serial.println("RF24Network/examples/helloworld_rx/");
 
  SPI.begin();
  radio.begin();
  network.begin(/*channel*/ 90, /*node address*/ hub);
}

void loop() {
  //receive data from your app, do not remove this line.
  chromeReceive();
  
  
  //Add your code here
  network.update();                  // Check the network regularly

  
  while ( network.available() ) {     // Is there anything ready for us?
    
    RF24NetworkHeader header;        // If so, grab it and print it out
    payload_t payload;
    network.read(header,&payload,sizeof(payload));
    
    if(payload.playerId == 1 && payload.hit == 1){
      p1_health = p1_health - damage;
    } else if (payload.playerId == 2 && payload.hit == 1){
      p2_health = p2_health - damage;
    }

    if(p1_health <= 0){
     Serial.println("Player 2 Wins!"); 
    } else if(p2_health <= 0){
     Serial.println("Player 1 Wins!"); 
    }   
    
    Serial.print("Received packet #");
    Serial.print(payload.counter);
    Serial.print(" at ");
    Serial.println(payload.ms);
    Serial.print("Player ");
    Serial.println(payload.playerId);
    Serial.print("Hit ");
    Serial.println(payload.hit);
    Serial.print("P1 Health: ");
    Serial.println(p1_health);
    Serial.print("P2 Health: ");
    Serial.println(p2_health);
  }
  
  //clear the fname to prevent from duplicating functions
  fname = "";
}

//----------------------
String V = "V";

void chromeSend(int pinNumber, int sendValue) {
  String A = "A";
  String E = "E";
  Serial.println(A + pinNumber + V + sendValue + E);
}

void chromeReceive() {
  String chrome;
  String pwm = "P";
  String dig = "D";
  String fn = "FN";
  int pin;
  int val;

  if (Serial.available() > 0) {
    String chrome = Serial.readStringUntil('\n');
    int chromeLen = chrome.length();

    if (chrome.indexOf(fn) == 0) {
      fname = chrome.substring(2, chromeLen);
    }
    else if (chrome.indexOf(dig) == 0 || chrome.indexOf(pwm) == 0 ){
      pin = chrome.substring(1, chrome.indexOf(V)).toInt();
      String valRaw = chrome.substring(chrome.indexOf(V) + 1, chromeLen);
      val = valRaw.toInt();

      if (autoPinMode) {
        pinMode(pin, OUTPUT);
      };

      if (directMode) {
        if (chrome.indexOf(dig) == 0) {
          digitalWrite(pin, val);
        }
        else if (chrome.indexOf(pwm) == 0 ) {
          analogWrite(pin, val);
        };
      }
      else {
        chromeDigital[pin] = val;
      };

    };
    
  };
};

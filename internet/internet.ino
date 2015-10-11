/*
  Repeating Web client

 This sketch connects to a a web server and makes a request
 using a Wiznet Ethernet shield. You can use the Arduino Ethernet shield, or
 the Adafruit Ethernet shield, either one will work, as long as it's got
 a Wiznet Ethernet module on board.

 This example uses DNS, by assigning the Ethernet client with a MAC address,
 IP address, and DNS address.

 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13

 created 19 Apr 2012
 by Tom Igoe
 modified 21 Jan 2014
 by Federico Vanzati

 http://arduino.cc/en/Tutorial/WebClientRepeating
 This code is in the public domain.

 */

#include <SPI.h>
#include <Ethernet.h>

#include <RF24Network.h>
#include <RF24.h>

RF24 radio(6,7);                // nRF24L01(+) radio attached using Getting Started board 

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

#define PUBLIC_KEY  "ro1EY6jAyaSZmGvy8W59" //Your SparkFun public_key
#define PRIVATE_KEY "jkelgEj2ARFA269Ya5Jv" //Your SparkFun private_key

// assign a MAC address for the ethernet controller.
// fill in your address here:
byte mac[] = {
  0x90, 0xA2, 0xDA, 0x0F, 0x98, 0xAD
};
// fill in an available IP address on your network here,
// for manual configuration:
//IPAddress ip(192, 168, 1, 177);

// fill in your Domain Name Server address here:
IPAddress myDns(1, 1, 1, 1);

// initialize the library instance:
EthernetClient client;

char server[] = "data.sparkfun.com";
//IPAddress server(64,131,82,241);

unsigned long lastConnectionTime = 0;             // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 10L * 1000L; // delay between updates, in milliseconds
// the "L" is needed to use long type numbers

void setup() {
  // start serial port:
  Serial.begin(57600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  SPI.begin();
  radio.begin();
  network.begin(/*channel*/ 90, /*node address*/ hub);  

  // give the ethernet module time to boot up:
  delay(1000);
  // start the Ethernet connection using a fixed IP address and DNS server:
  Ethernet.begin(mac);//, ip, myDns);
  // print the Ethernet board/shield's IP address:
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());
}

void loop() {
  
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
  
  // if there's incoming data from the net connection.
  // send it out the serial port.  This is for debugging
  // purposes only:
  if (client.available()) {
    char c = client.read();
    Serial.write(c);
  }

  // if ten seconds have passed since your last connection,
  // then connect again and send data:
  if (millis() - lastConnectionTime > postingInterval) {
    httpRequest();
  }

}

// this method makes a HTTP connection to the server:
void httpRequest() {
  // close any connection before send a new request.
  // This will free the socket on the WiFi shield
  client.stop();

  // if there's a successful connection:
  if (client.connect(server, 80)) {
    Serial.println("connecting...");
    // send the HTTP PUT request:
    client.print("GET /input/"); 
    client.print(PUBLIC_KEY);
    client.print("?private_key=");
    client.print(PRIVATE_KEY);
    client.print("&p1_health=");
    client.print(p1_health);
    client.print("&p2_health=");
    client.print(p2_health);
    client.println(" HTTP/1.1");
    client.println("Host: www.data.sparkfun.com");
    client.println("User-Agent: arduino-ethernet");
    client.println("Connection: close");
    client.println();

    // note the time that the connection was made:
    lastConnectionTime = millis();
  }
  else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
  }
}



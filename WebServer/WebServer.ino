/*
  Web Server

 A simple web server that shows the value of the analog input pins.
 using an Arduino Wiznet Ethernet shield.

 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 * Analog inputs attached to pins A0 through A5 (optional)

 created 18 Dec 2009
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe

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
int playerId;

struct payload_t {                 // Structure of our payload
  unsigned long ms;
  unsigned long counter;
  int playerId;
  bool hit;
};

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0x90, 0xA2, 0xDA, 0x0F, 0x98, 0xAD
};
IPAddress ip(10,20,31,20);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(57600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  SPI.begin();
  radio.begin();
  network.begin(/*channel*/ 90, /*node address*/ hub);

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}


void loop() {
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);

          // if you've gotten to the end of the line (received a newline
          // character) and the line is blank, the http request has ended,
          // so you can send a reply
          if (c == '\n' && currentLineIsBlank) {
            // send a standard http response header
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connection: close");  // the connection will be closed after completion of the response
            client.println("Refresh: 5");  // refresh the page automatically every 5 sec
            client.println();
            client.println("<!DOCTYPE HTML>");
            client.println("<html>");
              client.println("<head>");
                client.println("<title>ATW Laser Tag</title>");
                client.println("<style>");
                  client.println("body { background: whitesmoke;font-family:helvetica neue,sans-serif;}");
                  client.println("h1 {text-align: center}");
                  client.println("section {margin:30px auto;}");
                  client.println("article {text-align:center;}");
                  client.println("div {display:none;margin: 15px .5%;float:left;width:49%;overflow:hidden;text-align:center;box-shadow:0 5px 3px -3px #777;}");
                  client.println("div:first-of-type {display:block;background:#e53935;}");
                  client.println("div:nth-of-type(2) {display:block;background:#1e88e5;}");
                client.println("</style>");
               client.println("</head");
               client.println("<body>");
               client.println("<section>");
               client.println("<article>");
                 client.println("<h1>Amazing Laser Tag for ATW</h1>");
                 client.println("<h5>By Victor Cabieles and Cory Wilson</h5>");
               client.println("</article>");
            network.update(); 
            while ( network.available() ) {     // Is there anything ready for us?
              
              RF24NetworkHeader header;        // If so, grab it and print it out
              payload_t payload;
              network.read(header,&payload,sizeof(payload));
              
              if(payload.playerId == 1 && payload.hit == 1){
                p1_health = p1_health - damage;
              } else if (payload.playerId == 2 && payload.hit == 1){
                p2_health = p2_health - damage;
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
            
//              client.print("Received packet #");
//              client.print(payload.counter);
//              client.print(" at ");
//              client.print(payload.ms);
//              client.print("Player ");
//              client.print(payload.playerId);
//              client.print("| Hit ");
//              client.print(payload.hit);
              client.println("<div>");
                client.println("<p>");
                  client.println("P1 Health: ");
                client.println("</p>");   
                client.println("<h2>");             
                  client.println(p1_health);
                client.println("</h2>");
              client.println("</div>");
              client.println("<div>");
                client.println("<p>");
                  client.println("P2 Health: ");
                client.println("</p>");
                client.println("<h2>");
                  client.println(p2_health);
                client.println("</h2>");
              client.println("</div>");
              if(p1_health <= 0 && p2_health > 0){
               Serial.println("Player 2 Wins!");
               p1_health = 100;
               p2_health = 100;
               client.println("<h1>Player 2 Wins!</h1>"); 
              } else if(p2_health <= 0 && p1_health > 0){
               Serial.println("Player 1 Wins!"); 
               p1_health = 100;
               p2_health = 100;
               client.println("<h1>Player 1 Wins!</h1>"); 
              }   
            }
            // output the value of each analog input pin
//            for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
//              int sensorReading = analogRead(analogChannel);
//              client.print("analog input ");
//              client.print(analogChannel);
//              client.print(" is ");
//              client.print(sensorReading);
//              client.println("<br />");
//            }
              client.println("</section>");
              client.println("</body>");
            client.println("</html>");
            break;
          }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}


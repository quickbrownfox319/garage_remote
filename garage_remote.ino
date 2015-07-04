/*--------------------------------------------------------------
  Program:      eth_websrv_RELAY

  Description:  Arduino web server that serves up a web page
                allowing the user to control a RELAY
  
  Hardware:     - Arduino Uno and official Arduino Ethernet
                  shield. Should work with other Arduinos and
                  compatible Ethernet shields.
                - RELAY connected to 12v power supply
                - Genie garage remote
                
  Software:     Developed using Arduino 1.0.3 software
                Should be compatible with Arduino 1.0 +
  
  References:   - WebServer example by David A. Mellis and 
                  modified by Tom Igoe
                - Ethernet library documentation:
                  http://arduino.cc/en/Reference/Ethernet

  Date:         11 January 2013
 
  Author:       W.A. Smith, http://startingelectronics.com
--------------------------------------------------------------*/

//adjusted ethernet sketch example to control garage opener interface
//using an Arduino Uno, Seeed Studio ethernet shield, garage opener, and a relay.


#include <SPI.h>
#include <Ethernet.h>

// MAC address from Ethernet shield sticker under board
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,1,177); // IP address, may need to change depending on network
EthernetServer server(80);  // create a server at port 80
int remote = 5; //digital pin 5

String HTTP_req;          // stores the HTTP request

void setup()
{
    Ethernet.begin(mac, ip);  // initialize Ethernet device
    server.begin();           // start to listen for clients
    Serial.begin(9600);       // for diagnostics
    pinMode(remote, OUTPUT);       // relay on pin 5
}

void loop()
{
    EthernetClient client = server.available();  // try to get client

    if (client) {  // got client?
        boolean currentLineIsBlank = true;
        while (client.connected()) {
            if (client.available()) {   // client data available to read
                char c = client.read(); // read 1 byte (character) from client
                HTTP_req += c;  // save the HTTP request 1 char at a time
                // last line of client request is blank and ends with \n
                // respond to client only after last line received
                if (c == '\n' && currentLineIsBlank) {
                    // send a standard http response header
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type: text/html");
                    client.println("Connection: close");
                    client.println();
                    // send web page
                    client.println("<!DOCTYPE html>");
                    client.println("<html>");
                    client.println("<head>");
                    client.println("<meta content='width=device-width, initial-scale=1' name='viewport'/>");
                    client.println("<title>Arduino RELAY Control</title>");
                    client.println("</head>");
                    client.println("<body>");
                    client.println("<h1>Garage Opener</h1>");
                    client.println("<p>Activate to Power Relay</p>");
                    client.println("<form method=\"get\">");
                    ProcessButtonPress(client);
                    client.println("</form>");
                    client.println("</body>");
                    client.println("</html>");
                    Serial.print(HTTP_req);
                    HTTP_req = "";    // finished with request, empty string
                    break;
                }
                // every line of text received from the client ends with \r\n
                if (c == '\n') {
                    // last character on line of received text
                    // starting new line with next character read
                    currentLineIsBlank = true;
                } 
                else if (c != '\r') {
                    // a text character was received from client
                    currentLineIsBlank = false;
                }
            } // end if (client.available())
        } // end while (client.connected())
        delay(1);      // give the web browser time to receive the data
        client.stop(); // close the connection
    } // end if (client)
}

// switch RELAY and send back HTML for RELAY checkbox
void ProcessButtonPress(EthernetClient cl)
{
    cl.println(F("<input type=button value='Activate' onmousedown=location.href='/?REL_HIGH'>"));
    if(HTTP_req.indexOf("/?REL_HIGH")>0){
      digitalWrite(remote, HIGH);
      delay(100);
      digitalWrite(remote, LOW);
    }

}

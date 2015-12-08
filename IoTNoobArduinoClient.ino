
#include <SPI.h>
#include <Ethernet.h>
#include <Servo.h>

Servo myServo;
int pos=0;

byte mac[]    = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
EthernetClient client;

// Server address
IPAddress server(192,241,215,123);

// Vars to control time between requests
unsigned long lastConnectionTime = 0;
const unsigned long requestInterval = 1L * 1000L; // 1 Second

void setup() {
  myServo.attach(9);
  Serial.begin(9600);

  // Start ethernet DHCP
  Serial.println("Inicializando DHCP ...");
  if(Ethernet.begin(mac)) {
    Serial.println("DHCP Successful");
    Serial.println("IP Del Shield ethernet: " + Ethernet.localIP());
  }
  else {
    Serial.println("Fallo DHCP");
  }
}

void loop() {

  // Check if is there incomming trafic over network interface
  if(client.available()) {
    char c = client.read();
    if(c == '@') {
      char servoClockwise = client.read();
      if(servoClockwise == '1') {
        turnServoTill0();
      }
      else if(servoClockwise == '0') {
        turnServoTill180();
      }
      else {
        // Do nothing
        delay(1000);
      }
    }
  }

  // Its time to make request?
  if(millis() - lastConnectionTime > requestInterval) {
    makeHttpRequest();
  }
}

void makeHttpRequest() {
  // Close any open connection
  client.stop();

  if(client.connect(server, 80)){
    Serial.println("Connected to puños server");

    client.println("GET /api/servoDirection HTTP/1.1");
    client.println("Host: http://192.241.215.123");
    client.println("User-Agent: arduino-ethernet");
    client.println("Connection: close");
    client.println();
    
    Serial.println("Request sended");

    lastConnectionTime = millis();
  }
  else {
    Serial.println("Connection failed");
  }
}

void turnServoTill180() {

  // Take the servo from 0 to 180 with 1 second between 
  // each 15 degress.
  for(pos=0; pos<=180; pos+=15) {
    myServo.write(pos);
    delay(200);
  }
}

void turnServoTill0() {
  
  // Take the servo from 180 to 0 with 1 second between 
  // each 15 degress.
  for(pos=180; pos>=0; pos-=15) {
    myServo.write(pos);
    delay(200);
  }
}


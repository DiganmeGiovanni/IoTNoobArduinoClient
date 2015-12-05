
#include <SPI.h>
#include <Ethernet.h>
#include <Servo.h>

Servo myServo;
int pos = 0;

byte mac[]    = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
EthernetClient client;

// Server address
IPAddress server(192,168,100,8);

// Vars to control time between requests
unsigned long lastConnectionTime = 0;
const unsigned long requestInterval = 1L * 1000L; // 1 Second

void setup() {
  myServo.attach(9);
  Serial.begin(9600);

  // Start ethernet DHCP
  Serial.println("Inicializando DHCP");
  if(Ethernet.begin(mac)) {
    Serial.println("DHCP Successful");
    Serial.println("IP Del Shield ethernet: " + Ethernet.localIP());
  }
  else {
    Serial.println("Fallo DHCP");
  }
}

void loop() {

  // Check if there is incomming trafic over network interface
  if(client.available()) {
    char c = client.read();
    if(c == '@') {
      char to180 = client.read();
      if(to180 == '1') {
        turnServoTill180();
      }
      else {
        turnServoTill0();
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
    Serial.println("Connected to google server");

    client.println("GET /api/servoDirection HTTP/1.1");
    client.println("Host: http://192.168.100.8");
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
    delay(300);
  }
}

void turnServoTill0() {
  
  // Take the servo from 180 to 0 with 1 second between 
  // each 15 degress.
  for(pos=180; pos>=0; pos-=15) {
    myServo.write(pos);
    delay(300);
  }
}


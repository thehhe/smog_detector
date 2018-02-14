#include <SPI.h>
#include <Ethernet.h>
#include <aREST.h>
#include <avr/wdt.h>

//dust sensor definitions
#define MIN_VOLTAGE 600
#define VREF 1100
#define PIN_LED 7
#define PIN_ANALOG 0
#define MAX_ITERS 50

//dust sensor variables
int ADC_VALUE;
int ITER;
float VOLTAGE;
float DUST;
float AVG_DUST;

//establish REST server
aREST rest = aREST();
EthernetServer server = EthernetServer(80);

//connection data
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };  
byte ip[] = { 192, 168, 1, 5 };
byte my_dns[] = { 192, 168, 1, 1 };
byte gw[] = { 192, 168, 1, 1 };

float compute_dust();

void setup() {
  //diagnostics tool
  Serial.begin(9600); 

  //stable 1.1V
  analogReference(INTERNAL);

  //analog input
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);
  
  //assign action to rest request
  rest.function("dust", get_dust);
  rest.variable("ugm3", &AVG_DUST);
  
  //establish connection to LAN
  if(!Ethernet.begin(mac)) {
    Serial.println("DHCP failed. . .");
    Ethernet.begin(mac, ip, my_dns, gw);
  }
  server.begin();
  Serial.print("server IP: ");
  Serial.println(Ethernet.localIP());
  Serial.print("Setup complete\r\n");
}

void loop() {
  // put your main code here, to run repeatedly:
  EthernetClient client = server.available();
  rest.handle(client);
  wdt_reset();
}

float get_dust() {
  AVG_DUST = 0;
  ITER = 0;

  while (ITER < MAX_ITERS) {
    DUST = compute_dust();

    if (DUST > 0) {
      AVG_DUST += DUST;
      ITER++;
      delay(50);
    }
  }

  AVG_DUST /= MAX_ITERS;
  return AVG_DUST;
}

float compute_dust() {
  digitalWrite(PIN_LED, HIGH);
  delayMicroseconds(280);
  ADC_VALUE = analogRead(PIN_ANALOG);
  digitalWrite(PIN_LED, LOW);

  VOLTAGE = (VREF / 1024.0) * ADC_VALUE * 11;

  if (VOLTAGE > MIN_VOLTAGE)
    return (VOLTAGE - MIN_VOLTAGE) * 0.2;

  return 0;
}


// ArduinoJson - https://arduinojson.org
// Copyright Benoit Blanchon 2014-2021
// MIT License
//
// This example shows how to send a JSON document to a UDP socket.
// At regular interval, it sends a UDP packet that contains the status of
// analog and digital pins.
// It looks like that:
// {
//   "analog": [0, 76, 123, 158, 192, 205],
//   "digital": [1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0]
// }
//
// If you want to test this program, you need to be able to receive the UDP
// packets.
// For example, you can run netcat on your computer
// $ ncat -ulp 8888
// See https://nmap.org/ncat/
//
// https://arduinojson.org/v6/example/udp-beacon/

#include <ArduinoJson.h>
#include <Ethernet.h>
#include <SPI.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress remoteIp(192, 168, 0, 108);  // <- EDIT!!!!
unsigned short remotePort = 8888;
unsigned short localPort = 8888;
EthernetUDP udp;

void setup() {
  // Initialize serial port
  Serial.begin(9600);
  while (!Serial) continue;

  // Initialize Ethernet libary
  if (!Ethernet.begin(mac)) {
    Serial.println(F("Failed to initialize Ethernet library"));
    return;
  }

  // Enable UDP
  udp.begin(localPort);
}

void loop() {
  // Allocate a temporary JsonDocument
  // Use https://arduinojson.org/v6/assistant to compute the capacity.
  StaticJsonDocument<500> doc;

  // Create the "analog" array
  JsonArray analogValues = doc.createNestedArray("analog");
  for (int pin = 0; pin < 6; pin++) {
    // Read the analog input
    int value = analogRead(pin);

    // Add the value at the end of the array
    analogValues.add(value);
  }

  // Create the "digital" array
  JsonArray digitalValues = doc.createNestedArray("digital");
  for (int pin = 0; pin < 14; pin++) {
    // Read the digital input
    int value = digitalRead(pin);

    // Add the value at the end of the array
    digitalValues.add(value);
  }

  // Log
  Serial.print(F("Sending to "));
  Serial.print(remoteIp);
  Serial.print(F(" on port "));
  Serial.println(remotePort);
  serializeJson(doc, Serial);

  // Send UDP packet
  udp.beginPacket(remoteIp, remotePort);
  serializeJson(doc, udp);
  udp.println();
  udp.endPacket();

  // Wait
  delay(10000);
}

// Performance issue?
// ------------------
//
// EthernetUDP is an unbuffered stream, which is not optimal for ArduinoJson.
// See: https://arduinojson.org/v6/how-to/improve-speed/

// See also
// --------
//
// https://arduinojson.org/ contains the documentation for all the functions
// used above. It also includes an FAQ that will help you solve any
// serialization problem.
//
// The book "Mastering ArduinoJson" contains a tutorial on serialization.
// It begins with a simple example, then adds more features like serializing
// directly to a file or any stream.
// Learn more at https://arduinojson.org/book/
// Use the coupon code TWENTY for a 20% discount ❤❤❤❤❤

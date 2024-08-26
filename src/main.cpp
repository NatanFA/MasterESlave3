//Comente ou remova para compilar o Slave
#define MASTER

#ifdef MASTER

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include "SSD1306.h"
#include <RadioLib.h>

//Deixe esta linha descomentada para compilar o Master
//Comente ou remova para compilar o Slave

// Pin Definitions for LoRa module
#define LoRa_MOSI 10
#define LoRa_MISO 11
#define LoRa_SCK 9

#define LoRa_nss 8
#define LoRa_dio1 14
#define LoRa_nrst 12
#define LoRa_busy 13

SSD1306 display(0x3c, 17, 18, 21);

// Create an instance of the LoRa module
SX1262 radio = new Module(LoRa_nss, LoRa_dio1, LoRa_nrst, LoRa_busy);

#define BAND 868E6

void setup(){
//O estado do GPIO16 é utilizado para controlar o display OLED
  Serial.begin(9600);
  pinMode(36, OUTPUT);
  //Reseta as configurações do display OLED
  digitalWrite(36, LOW);
  delay(50);
  //Para o OLED permanecer ligado, o GPIO16 deve permanecer HIGH
  //Deve estar em HIGH antes de chamar o display.init() e fazer as demais configurações,
  //não inverta a ordem
  digitalWrite(36, HIGH);
  Serial.println("Iniciei");
  //Configurações do display
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.clear();

  // Initialize SPI communication for LoRa module
  SPI.begin(LoRa_SCK, LoRa_MISO, LoRa_MOSI, LoRa_nss);

  // Initialize LoRa module
  Serial.print(("[SX1262] Initializing ... "));
  int state = radio.begin();
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(("success!"));
  } else {
    Serial.print(("failed, code "));
    Serial.println(state);
    while (true);
  }

  // Set the frequency (in Hz)
  // Range: 150-960 MhZ 900 MHz frequency
  radio.setFrequency(BAND);  // 900 MhZ

  // Set other configurations as needed
  
  display.clear();
  //display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 0, "Master");
  display.display();
}

void loop(){
  // Transmit a message
  Serial.print(("[SX1262] Transmitting packet … "));
  // Send the message
  int state = radio.transmit("getdata");

  // Check transmission status
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(("success!"));
  } else {
    Serial.print(("failed, code "));
    Serial.println(state);
  }

  delay(500); // Wait for 5 seconds before transmitting again
}

#endif

#ifndef MASTER

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <RadioLib.h>

#define LoRa_MOSI 10
#define LoRa_MISO 11
#define LoRa_SCK 9

#define LoRa_nss 8
#define LoRa_dio1 14
#define LoRa_nrst 12
#define LoRa_busy 13

SX1262 radio = new Module(LoRa_nss, LoRa_dio1, LoRa_nrst, LoRa_busy);

void setup() {
Serial.begin(9600);
Serial.println(F("Serial communication initialized."));

SPI.begin(LoRa_SCK, LoRa_MISO, LoRa_MOSI, LoRa_nss);
Serial.println(F("SPI communication initialized."));

// Initialize LoRa module
Serial.print(F("[SX1262] Initializing ... "));
int state = radio.begin();
if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("Initialization successful!"));
} else {
    Serial.print(F("Initialization failed, code "));
    Serial.println(state);
    while (true);
}

// Set the frequency (in Hz)
// Range: 150-960 MhZ 900 MHz frequency
radio.setFrequency(915000000);  // 433 MHz
}

void loop() {
// Receive a packet

String str;
int state = radio.receive(str);

if (state == RADIOLIB_ERR_NONE) {
    // Packet received successfully
    Serial.println(F("Received packet successfully!"));


    // Print the RSSI (Received Signal Strength Indicator) of the last received packet
    Serial.print(F("[SX1262] RSSI:\t\t"));
    Serial.print(radio.getRSSI());
    Serial.println(F(" dBm"));

    // Print the SNR (Signal-to-Noise Ratio) of the last received packet
    Serial.print(F("[SX1262] SNR:\t\t"));
    Serial.print(radio.getSNR());
    Serial.println(F(" dB"));


    // Print the received data
    Serial.print(F("[SX1262] Voltage:\t\t"));
    Serial.println(str);

    //Print the voltage
    //Serial.print(F("[SX1262] Voltage:\t\t"));
    //Serial.println(str);

} else if (state != RADIOLIB_ERR_RX_TIMEOUT && state != RADIOLIB_ERR_CRC_MISMATCH) {
    // Some other error occurred (excluding timeout and CRC mismatch)
    Serial.print(F("Failed to receive packet, code "));
    Serial.println(state);
}
}
#endif

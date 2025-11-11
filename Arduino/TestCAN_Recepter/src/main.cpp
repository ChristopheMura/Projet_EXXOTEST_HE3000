#include <Arduino.h>
#include <mcp_can.h>
#include <SPI.h>

extern "C" {
#include "base_de_donnee_can_he3000.h"
}

const int SPI_CS_PIN = 10;
MCP_CAN mcp2515(SPI_CS_PIN);

void setup() {
  Serial.begin(115200);
  
  // IMPORTANT : Même vitesse et fréquence que l'émetteur !
  while (mcp2515.begin(MCP_ANY, CAN_250KBPS, MCP_16MHZ) != CAN_OK) {
    Serial.println("Erreur : Initialisation MCP2515...");
    delay(1000);
  }
  
  Serial.println("MCP2515 recepteur initialise !");
  mcp2515.setMode(MCP_NORMAL); // Mode normal
}

void loop() {
  long unsigned int rxId;
  unsigned char len;
  unsigned char rxBuf[8];
  
  if (mcp2515.checkReceive() == CAN_MSGAVAIL) {
    mcp2515.readMsgBuf(&rxId, &len, rxBuf);
    
    Serial.print("Message recu - ID: 0x");
    Serial.print(rxId, HEX);
    Serial.print(" | Data: ");
    
    for (int i = 0; i < len; i++) {
      if (rxBuf[i] < 0x10) Serial.print("0");
      Serial.print(rxBuf[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
    
    // Décodage si c'est le bon message
    if (rxId == BASE_DE_DONNEE_CAN_HE3000_INFO_1_FRAME_ID) {
      base_de_donnee_can_he3000_info_1_t msg;
      base_de_donnee_can_he3000_info_1_unpack(&msg, rxBuf, len);
      
      Serial.println("--- Message decode ---");
      Serial.print("  BDI Percentage: ");
      Serial.println(msg.bdi_percentage);
      Serial.print("  Controller Status: ");
      Serial.println(msg.controller_status);
      Serial.print("  Total Odometer: ");
      Serial.println(msg.total_odometer);
      Serial.print("  Vehicle Speed: ");
      Serial.println(msg.vehicle_speed);
      Serial.println();
    }
  }
}

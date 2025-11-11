#include <Arduino.h>
#include <mcp_can.h>
#include <SPI.h>

extern "C" {
  #include "base_de_donnee_can_he3000.h"
}

const int SPI_CS_PIN = 9; // Pin CS
MCP_CAN mcp2515(SPI_CS_PIN);

void setup()
{
  Serial.begin(115200);
  while (mcp2515.begin(MCP_ANY, CAN_250KBPS, MCP_16MHZ) != CAN_OK)
  {
    Serial.println("Erreur : Initialisation MCP2515...");
    delay(1000);
  }
  Serial.println("MCP2515 emetteur initialisé !");
  mcp2515.setMode(MCP_NORMAL); // Mode normal pour envoyer/recevoir
}

void loop()
{
  base_de_donnee_can_he3000_info_1_t msg;
  msg.bdi_percentage = 63;
  msg.controller_status = 1;
  msg.total_odometer = 100245;
  msg.vehicle_speed = 123;

  uint8_t data[8];
  int len = base_de_donnee_can_he3000_info_1_pack(data, &msg, sizeof(data));

  byte status = mcp2515.sendMsgBuf(BASE_DE_DONNEE_CAN_HE3000_INFO_1_FRAME_ID, 0, len, data);

  if (status == CAN_OK)
  {
    Serial.println("Message envoye");
  }
  else
  {
    Serial.print("Erreur envoie : ");
    Serial.println(status);
  }
  delay(500);
}

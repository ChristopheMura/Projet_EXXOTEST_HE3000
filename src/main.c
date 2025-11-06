#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>

#include "../messageries/base_de_donnee_can_he3000.h"

// Fonction pour afficher les données brutes reçues
void afficher_trame_brute(const uint8_t *data, uint8_t length);

// Fonction pour décoder et afficher le message Info_1
void decoder_info_1(const uint8_t *data, uint8_t dlc);

int main(int argc, char** argv)
{
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════╗\n");
    printf("║     PROGRAMME D'ACQUISITION - MESSAGE INFO_1          ║\n");
    printf("╚═══════════════════════════════════════════════════════╝\n");
    printf("\n");

    // ========================================================================
    // SIMULATION DE RÉCEPTION DE TRAMES CAN
    // ========================================================================
    // Dans un vrai système, ces données viendraient du bus CAN via SocketCAN
    // ========================================================================
    
    printf("Simulation de réception de différentes trames Info_1:\n");
    printf("════════════════════════════════════════════════════════\n\n");
    
    // ─── Exemple 1 : Véhicule en mouvement normal ───
    printf("═══ Exemple 1 : Véhicule en mouvement ═══\n");
    uint8_t trame1[] = {0x57, 0x03, 0x3F, 0xE2, 0x01, 0x00, 0x01, 0x4E};
    decoder_info_1(trame1, 8);
    
    // ─── Exemple 2 : Véhicule à l'arrêt ───
    printf("═══ Exemple 2 : Véhicule à l'arrêt ═══\n");
    uint8_t trame2[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x64};
    decoder_info_1(trame2, 8);
    
    // ─── Exemple 3 : Marche arrière ───
    printf("═══ Exemple 3 : Marche arrière ═══\n");
    uint8_t trame3[] = {0x2C, 0x01, 0x39, 0x30, 0x00, 0x00, 0x02, 0x4B};
    decoder_info_1(trame3, 8);
    
    // ─── Exemple 4 : Batterie faible ───
    printf("═══ Exemple 4 : Batterie faible ═══\n");
    uint8_t trame4[] = {0xB0, 0x04, 0x20, 0xA1, 0x07, 0x00, 0x01, 0x0A};
    decoder_info_1(trame4, 8);

    return 0;
}

// Fonction pour afficher les données brutes reçues
void afficher_trame_brute(const uint8_t *data, uint8_t length)
{
    printf("  Trame brute (hex) : ");
    for (int i = 0; i < length; i++) {
        printf("%02X ", data[i]);
    }
    printf("\n");
}

// Fonction pour décoder et afficher le message Info_1
void decoder_info_1(const uint8_t *data, uint8_t dlc)
{
    printf("╔═══════════════════════════════════════════════════════╗\n");
    printf("║       DÉCODAGE DU MESSAGE INFO_1 (ID: 0x227)          ║\n");
    printf("╚═══════════════════════════════════════════════════════╝\n\n");
    
    // Afficher la trame brute reçue
    afficher_trame_brute(data, dlc);
    printf("\n");
    
    // Décoder la trame CAN
    struct base_de_donnee_can_he3000_info_1_t message;
    
    int resultat = base_de_donnee_can_he3000_info_1_unpack(&message, data, dlc);
    
    if (resultat != 0) {
        printf("  ❌ ERREUR de décodage: %d\n", resultat);
        return;
    }
    
    printf("  ✅ Décodage réussi!\n\n");
    
    // Convertir en valeurs physiques
    float vitesse = base_de_donnee_can_he3000_info_1_vehicle_speed_decode(message.vehicle_speed);
    float odometer = base_de_donnee_can_he3000_info_1_total_odometer_decode(message.total_odometer);
    uint8_t statut = message.controller_status;
    uint8_t batterie = message.bdi_percentage;
    
    // Afficher les signaux décodés
    printf("  📊 SIGNAUX DÉCODÉS:\n");
    printf("  ─────────────────────────────────────────────────────\n");
    printf("  🚗 Vitesse véhicule   : %.1f km/h\n", vitesse);
    printf("  📏 Odomètre total     : %.1f km\n", odometer);
    printf("  🔄 Statut contrôleur  : %u (%s)\n", statut,
           statut == 0 ? "Neutre" : 
           statut == 1 ? "Marche avant" : 
           statut == 2 ? "Marche arrière" : "Inconnu");
    printf("  🔋 Niveau batterie    : %u%%\n", batterie);
    printf("\n");
    
    // Afficher les alertes si nécessaire
    if (batterie < 20) {
        printf("  ⚠️  ALERTE: Niveau de batterie faible!\n");
    }
    if (vitesse > 100.0) {
        printf("  ⚠️  ALERTE: Vitesse élevée!\n");
    }
}
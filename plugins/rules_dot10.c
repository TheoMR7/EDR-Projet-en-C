/*
 * rules_dot10.c
 * -------------
 * Extension dynamique (.so) pour le filtrage heuristique des adresses IPv4
 * dont le premier ou le dernier octet vaut 10.
 * Le moteur charge plugin_check_packet() avec dlopen() et dlsym().
 */
#include "../include/packet.h"
#include "../include/state_table.h"
#include "../include/rules_engine.h"
#include <stdio.h>
#include <string.h>

int plugin_check_packet(const Packet * pkt, const SessionEntry * session, Alert * out_alert) {
    unsigned char first_octet;
    unsigned char last_octet;
    /*analyse stateless (sans état) uniquement sur l'en-tête IP*/
    (void)session; 

    if (pkt == NULL || out_alert == NULL) {
        return 0;
    }
    /*Extraction bit-à-bit en temps constant*/
    first_octet = (unsigned char)((pkt->src_ip >> 24) & 0xFF);
    last_octet  = (unsigned char)(pkt->src_ip & 0xFF);
    /*vérifie si l'IP appartient à un sous-réseau privé pour avoir que la classe A*/
    if (first_octet == 10 || last_octet == 10) {
        out_alert->timestamp_ms = pkt->timestamp_ms;
        out_alert->src_ip       = pkt->src_ip;
        out_alert->dst_ip       = pkt->dst_ip;
        out_alert->severity     = ALERT_WARNING;
        /* Copie sécurisée anti-buffer overflow*/
        strncpy(out_alert->rule_name, "Plugin_Heuristique_Dot10", sizeof(out_alert->rule_name) - 1);
        out_alert->rule_name[sizeof(out_alert->rule_name) - 1] = '\0';
        /*Formatage borné dans le tampon de destination*/
        snprintf(out_alert->message, sizeof(out_alert->message),
                 "[PLUGIN .SO] Flux suspect : IP source %u.x.x.%u detectee hors Whitelist",
                 (unsigned int)first_octet, (unsigned int)last_octet);

        return 1;
    }

    return 0;
}
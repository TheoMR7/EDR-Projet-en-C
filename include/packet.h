/*
 * packet.h
 * --------
 * Contrat commun representant un paquet reseau deja decode.
 * Toutes les autres structures doivent utiliser ces definitions afin que
 * les modules de capture, de suivi d'etat et de regles restent compatibles.
 */
#ifndef PACKET_H
#define PACKET_H

#include <stdio.h>
#include <stdlib.h>

/* Identifie le protocole transporte par le paquet. */
typedef enum ProtocolType ProtocolType;
enum ProtocolType {
    PROTO_UNKNOWN = 0,
    PROTO_ICMP    = 1,
    PROTO_TCP     = 6,
    PROTO_UDP     = 17
};

/* Masques permettant de tester les differents bits des drapeaux TCP. */
#define TCP_FLAG_FIN 0x01
#define TCP_FLAG_SYN 0x02
#define TCP_FLAG_RST 0x04
#define TCP_FLAG_PSH 0x08
#define TCP_FLAG_ACK 0x10
#define TCP_FLAG_URG 0x20

typedef struct Packet Packet;
struct Packet {
    /* Adresses IPv4 source et destination sur 32 bits. */
    unsigned int   src_ip;
    unsigned int   dst_ip;
    /* Ports source et destination pour TCP ou UDP. */
    unsigned short src_port;
    unsigned short dst_port;
    /* Protocole IP observe. */
    ProtocolType   protocol;
    /* Ensemble des bits d'etat TCP presents dans l'en-tete. */
    unsigned char  tcp_flags;
    /* Taille utile des donnees transportees. */
    unsigned int   payload_size;
    /* Moment de reception du paquet en millisecondes. */
    unsigned long  timestamp_ms;
};

#endif
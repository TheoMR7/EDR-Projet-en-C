#ifndef CAPTURE_H
#define CAPTURE_H

/*
 * capture.h
 * ---------
 * Declaration du module charge de la capture brute des paquets sous Linux.
 * Les constantes et bibliotheques incluses permettent d'ouvrir un socket
 * AF_PACKET, de recevoir les octets et de manipuler les en-tetes reseau.
 */

/* Bibliotheques standard, reseau et Unix necessaires a la capture. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <sys/socket.h>

#define BUFFER_SIZE 65536

/*
 * Lance la capture de demonstration en boucle infinie.
 * Cette fonction doit etre arretee par la fermeture du processus ou un signal.
 */
int capture_debug_run(void);
#endif

#ifndef ALERT_H
#define ALERT_H

/*
 * alert.h
 * -------
 * Structures communes aux modules qui produisent ou affichent des alertes.
 * Une alerte conserve l'instant, les adresses concernées, sa gravite,
 * le nom de la regle declenchee et un message descriptif.
 */

/* Niveaux possibles pour qualifier l'importance d'une alerte. */
typedef enum AlertSeverity {
    ALERT_INFO = 1,
    ALERT_WARNING = 2,
    ALERT_CRITICAL = 3
} AlertSeverity;

/* Description complete d'un evenement suspect detecte. */
typedef struct Alert {
    /* Horodatage en millisecondes depuis l'Epoch Unix. */
    unsigned long timestamp_ms;
    /* Adresses IPv4 numeriques source et destination. */
    unsigned int src_ip;
    unsigned int dst_ip;
    /* Gravite de l'evenement. */
    AlertSeverity severity;
    /* Nom court de la regle qui a declenche l'alerte. */
    char rule_name[64];
    /* Explication lisible de l'evenement. */
    char message[256];
} Alert;

/* Affiche une alerte avec sa date, ses adresses IP et ses details. */
void alert_print(Alert alert);

/* Ajoute une alerte au fichier JSON en une seule ligne. */
int alert_log_json(Alert alert, const char filepath[]);

/* Fonction de rappel affiche l'alerte et l'enregistre si un chemin est fourni. */
void alert_callback_json(const Alert *alert, void *user_context);

#endif

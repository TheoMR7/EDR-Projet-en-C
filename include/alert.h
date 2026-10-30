#ifndef ALERT_H
#define ALERT_H

#include <time.h>

typedef struct Alert Alert;

struct Alert {
    time_t timestamp;
    char severity[16];
    char src_ip[16];
    char dst_ip[16];
    char message[128];
};

/* On transmet directement l'alerte aux fonctions. */
void alert_print(Alert alert);

/* Renvoie 1 si l'enregistrement reussit, sinon 0. */
int alert_log_json(Alert alert, const char filepath[]);

#endif

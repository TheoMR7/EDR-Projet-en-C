#include "alert.h"
#include <stdio.h>

int main(void)
{
    Alert alert = {
        .severity = "ELEVEE",
        .src_ip = "192.168.1.10",
        .dst_ip = "192.168.1.20",
        .message = "Scan SYN detecte"
    };

    alert.timestamp = time(NULL);
    alert_print(alert);

    if (alert_log_json(alert, "alerts.json") == 0) {
        fprintf(stderr, "Erreur : impossible d'enregistrer l'alerte.\n");
        return 1;
    }

    return 0;
}

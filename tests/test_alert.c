
#include <stdio.h>
#include <time.h>
#include "alert.h"
#include "state_table.h"

int main(void)
{
    Alert alert = {0};

    alert.timestamp_ms = (unsigned long)time(NULL) * 1000UL;

    alert.severity = ALERT_CRITICAL;

    alert.src_ip = ip_from_bytes(192, 168, 1, 10);
    alert.dst_ip = ip_from_bytes(192, 168, 1, 20);

    snprintf(alert.rule_name,
             sizeof(alert.rule_name),
             "%s", "Detection_SYN_Scan");

    snprintf(alert.message,
             sizeof(alert.message),
             "%s", "Scan SYN detecte");

    alert_print(alert);

    if (alert_log_json(alert, "alerts.json") == 0) {
        printf("Erreur lors de l'enregistrement.\n");
        return 1;
    }

    return 0;
}

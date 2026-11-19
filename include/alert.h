
#ifndef ALERT_H
#define ALERT_H

typedef enum AlertSeverity {
    ALERT_INFO = 1,
    ALERT_WARNING = 2,
    ALERT_CRITICAL = 3
} AlertSeverity;

typedef struct Alert {
    unsigned long timestamp_ms;
    unsigned int src_ip;
    unsigned int dst_ip;
    AlertSeverity severity;
    char rule_name[64];
    char message[256];
} Alert;

void alert_print(Alert alert);
int alert_log_json(Alert alert, const char filepath[]);

void alert_callback_json(const Alert *alert, void *user_context);

#endif

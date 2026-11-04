/*Moteur de règles modulaire, pointeurs de fonctions et plugins (.so).*/
#ifndef RULES_ENGINE_H
#define RULES_ENGINE_H

#include "packet.h"
#include "state_table.h"

typedef enum AlertSeverity AlertSeverity;
enum AlertSeverity {
    ALERT_INFO     = 1,
    ALERT_WARNING  = 2,
    ALERT_CRITICAL = 3
};

typedef struct Alert Alert;
struct Alert {
    unsigned long timestamp_ms;
    unsigned int  src_ip;
    unsigned int  dst_ip;
    AlertSeverity severity;
    char          rule_name[64];
    char          message[256];
};
typedef int (*RuleCheckFunc)(const Packet * pkt, const SessionEntry * session, Alert * out_alert);

typedef struct DetectionRule DetectionRule;
struct DetectionRule {
    int           id;
    char          name[64];
    int           enabled;
    RuleCheckFunc check;
    void *        dl_handle;
};
typedef void (*AlertCallback)(const Alert * alert, void * user_context);

typedef struct RulesEngine RulesEngine;
struct RulesEngine {
    IpNode *        whitelist;
    IpNode *        blacklist;
    int             auto_ban_attackers;
    DetectionRule * rules;
    int             rules_len;
    int             rules_capacity;
    unsigned int    syn_scan_threshold;
    unsigned long   dos_bytes_threshold;
    AlertCallback   on_alert;
    void *          alert_ctx;
};

void engine_init(RulesEngine * engine, AlertCallback callback, void * callback_ctx);
int  engine_load_configs(RulesEngine * engine, const char * wl_path, const char * bl_path);
void engine_free(RulesEngine * engine);

int  engine_add_rule(RulesEngine * engine, const char * name, RuleCheckFunc check_func);
int  engine_load_plugin(RulesEngine * engine, const char * so_filepath, const char * rule_name);
int  engine_set_rule_state(RulesEngine * engine, int rule_id, int enabled);
int  engine_inspect_packet(RulesEngine * engine, const Packet * pkt, const SessionEntry * session);

int  rule_check_syn_scan(const Packet * pkt, const SessionEntry * session, Alert * out_alert);
int  rule_check_dos_volume(const Packet * pkt, const SessionEntry * session, Alert * out_alert);
int  rule_check_dot10_ip(const Packet * pkt, const SessionEntry * session, Alert * out_alert);

#endif
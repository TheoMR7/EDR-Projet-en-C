#ifndef RULES_H
#define RULES_H

#include <stdbool.h>
#include <stddef.h>
#include "packet.h"
#include "state_table.h"

typedef enum {
    SEVERITY_LOW,
    SEVERITY_MEDIUM,
    SEVERITY_HIGH,
    SEVERITY_CRITICAL
} AlertSeverity;

typedef struct {
    char rule_name[64];
    char description[256];
    AlertSeverity severity;
    uint32_t src_ip;
    uint32_t dst_ip;
} Alert;

typedef bool (*RuleEngineFunc)(const Packet *pkt, const StateTable *table, Alert *out_alert);

typedef struct {
    int rule_id;
    char name[64];
    bool enabled;
    RuleEngineFunc check_func;
} Rule;

#define MAX_RULES 32

typedef struct {
    Rule rules[MAX_RULES];
    size_t count;
} RuleSet;

void rules_init(RuleSet *rs);
bool rules_add(RuleSet *rs, int id, const char *name, RuleEngineFunc func);
void rules_evaluate(const RuleSet *rs, const Packet *pkt, const StateTable *table);

bool rule_check_syn_scan(const Packet *pkt, const StateTable *table, Alert *out_alert);
bool rule_check_volumetric_spike(const Packet *pkt, const StateTable *table, Alert *out_alert);

#endif
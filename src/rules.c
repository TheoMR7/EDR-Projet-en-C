#include <stdio.h>
#include <string.h>
#include "rules.h"

void rules_init(RuleSet *rs) {
    if (rs == NULL) return;
    rs->count = 0;
    for (int i = 0; i < MAX_RULES; i++) {
        rs->rules[i].enabled = false;
        rs->rules[i].rule_id = 0;
        rs->rules[i].name[0] = '\0';
        rs->rules[i].check_func = NULL;
    }
}

bool rules_add(RuleSet *rs, int id, const char *name, RuleEngineFunc func) {
    if (rs == NULL || name == NULL || func == NULL) return false;
    if (rs->count >= MAX_RULES) return false;

    rs->rules[rs->count].rule_id = id;
    strncpy(rs->rules[rs->count].name, name, sizeof(rs->rules[rs->count].name) - 1);
    rs->rules[rs->count].name[sizeof(rs->rules[rs->count].name) - 1] = '\0';
    rs->rules[rs->count].enabled = true;
    rs->rules[rs->count].check_func = func;
    
    rs->count++;
    return true;
}

void rules_evaluate(const RuleSet *rs, const Packet *pkt, const StateTable *table) {
    if (rs == NULL || pkt == NULL) return;

    for (size_t i = 0; i < rs->count; i++) {
        if (rs->rules[i].enabled && rs->rules[i].check_func != NULL) {
            Alert alert;
            if (rs->rules[i].check_func(pkt, table, &alert)) {
                printf("[ALERTE DÉTECTÉE] Règle: %s | %s (Sévérité: %d)\n", 
                       rs->rules[i].name, alert.description, alert.severity);
            }
        }
    }
}

bool rule_check_syn_scan(const Packet *pkt, const StateTable *table, Alert *out_alert) {
    if (pkt == NULL || out_alert == NULL) return false;
    
    (void)table; 

    return false;
}

bool rule_check_volumetric_spike(const Packet *pkt, const StateTable *table, Alert *out_alert) {
    if (pkt == NULL || out_alert == NULL) return false;
    (void)table;

    return false;
}
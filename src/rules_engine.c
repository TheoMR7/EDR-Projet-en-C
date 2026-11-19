/*
 * rules_engine.c
 * --------------
 * Implementation du moteur de regles, de la reputation IP et des plugins .so.
 * Il initialise les regles integrees, charge les configurations, charge des
 * modules dynamiques et inspecte chaque paquet par rapport aux regles actives.
 */

#include "../include/rules_engine.h"
#include <string.h>
#include <dlfcn.h>
/* Fonction de rappel par défaut pour l'affichage des alertes sur la sortie standard.*/
static void default_alert_printer(const Alert * alert, void * user_context) {
    char src_str[16];
    char dst_str[16];
    const char * sev_str = "INFO";
/* Le contexte utilisateur n'est pas utilisé dans cette fonction par défaut.*/
    (void)user_context;
    if (alert == NULL) {
        return;
    }
    if (alert->severity == ALERT_WARNING) {
        sev_str = "WARNING";
    } else if (alert->severity == ALERT_CRITICAL) {
        sev_str = "CRITICAL";
    }

    ip_to_string(alert->src_ip, src_str, sizeof(src_str));
    ip_to_string(alert->dst_ip, dst_str, sizeof(dst_str));

    printf("[ALERT %s] REGLE: '%s' | %s -> %s | %s\n",
           sev_str, alert->rule_name, src_str, dst_str, alert->message);
}
/* Fonction de vérification de la règle intégrée pour la détection de scans SYN.*/
void engine_init(RulesEngine * engine, AlertCallback callback, void * callback_ctx) {
    if (engine == NULL) {
        return;
    }
    engine->whitelist           = NULL;
    engine->blacklist           = NULL;
    engine->auto_ban_attackers  = 1;
    engine->rules               = NULL;
    engine->rules_len           = 0;
    engine->rules_capacity      = 0;
    engine->syn_scan_threshold  = 10;
    engine->dos_bytes_threshold = 1000000UL;

    if (callback != NULL) {
        engine->on_alert  = callback;
        engine->alert_ctx = callback_ctx;
    } else {
        engine->on_alert  = default_alert_printer;
        engine->alert_ctx = NULL;
    }
/* Ajout des règles intégrées au moteur de règles.*/
    engine_add_rule(engine, "Detection_SYN_Scan", rule_check_syn_scan);
    engine_add_rule(engine, "Detection_Pic_Volumetrique_DoS", rule_check_dos_volume);
    engine_add_rule(engine, "Heuristique_IP_Point10", rule_check_dot10_ip);
}
/* Chargement des fichiers de configuration pour la liste blanche et la liste noire.*/
int engine_load_configs(RulesEngine * engine, const char * wl_path, const char * bl_path) {
    int wl_count = 0;
    int bl_count = 0;

    if (engine == NULL) {
        return -1;
    }
    if (wl_path != NULL) {
        wl_count = ip_tree_load_file(&(engine->whitelist), wl_path);
    }
    if (bl_path != NULL) {
        bl_count = ip_tree_load_file(&(engine->blacklist), bl_path);
    }
    return (wl_count >= 0 && bl_count >= 0) ? (wl_count + bl_count) : -1;
}
/* Libération des ressources allouées par le moteur de règles.*/
int engine_add_rule(RulesEngine * engine, const char * name, RuleCheckFunc check_func) {
    DetectionRule * new_array = NULL;
    int new_cap;
    DetectionRule * rule = NULL;

    if (engine == NULL || name == NULL || check_func == NULL) {
        return 0;
    }
    if (engine->rules_len >= engine->rules_capacity) {
        new_cap = (engine->rules_capacity == 0) ? 4 : (engine->rules_capacity * 2);
        new_array = (DetectionRule *)realloc(engine->rules, (size_t)new_cap * sizeof(DetectionRule));
        if (new_array == NULL) {
            fprintf(stderr, "Erreur realloc dans engine_add_rule\n");
            return 0;
        }
        engine->rules = new_array;
        engine->rules_capacity = new_cap;
    }
/* Ajout de la nouvelle règle à la fin du tableau des règles.*/
    rule = &(engine->rules[engine->rules_len]);
    rule->id = engine->rules_len + 1;
    strncpy(rule->name, name, sizeof(rule->name) - 1);
    rule->name[sizeof(rule->name) - 1] = '\0';
    rule->enabled   = 1;
    rule->check     = check_func;
    rule->dl_handle = NULL;

    ++(engine->rules_len);
    return rule->id;
}
/* Chargement d'un plugin dynamique (.so) et ajout de la règle correspondante au moteur de règles.*/
int engine_load_plugin(RulesEngine * engine, const char * so_filepath, const char * rule_name) {
    void * handle = NULL;
    RuleCheckFunc plugin_func = NULL;
    int rule_id;

    if (engine == NULL || so_filepath == NULL || rule_name == NULL) {
        return 0;
    }

    handle = dlopen(so_filepath, RTLD_NOW);
    if (handle == NULL) {
        fprintf(stderr, "Erreur dlopen (%s) : %s\n", so_filepath, dlerror());
        return 0;
    }
    *(void **)(&plugin_func) = dlsym(handle, "plugin_check_packet"); /* Récupération de la fonction de vérification du plugin dynamique.*/
    if (plugin_func == NULL) {
        fprintf(stderr, "Erreur dlsym : fonction 'plugin_check_packet' introuvable (%s)\n", dlerror());
        dlclose(handle);
        return 0;
    }
    /* Ajout de la règle du plugin au moteur de règles.*/
    rule_id = engine_add_rule(engine, rule_name, plugin_func);
    if (rule_id == 0) {
        dlclose(handle);
        return 0;
    }
    engine->rules[engine->rules_len - 1].dl_handle = handle;
    return rule_id;
}
/* Activation ou désactivation d'une règle spécifique dans le moteur de règles.*/
int engine_set_rule_state(RulesEngine * engine, int rule_id, int enabled) {
    int i;

    if (engine == NULL) {
        return 0;
    }
    for (i = 0; i < engine->rules_len; ++i) {
        if (engine->rules[i].id == rule_id) {
            engine->rules[i].enabled = (enabled != 0) ? 1 : 0;
            return 1;
        }
    }
    return 0;
}
/* Inspection d'un paquet réseau en utilisant le moteur de règles et génération d'alertes si nécessaire.*/
int engine_inspect_packet(RulesEngine * engine, const Packet * pkt, const SessionEntry * session) {
    int i;
    int alerts_triggered = 0;
    Alert alert;

    if (engine == NULL || pkt == NULL) {
        return 0;
    }

    if (ip_tree_contains(engine->whitelist, pkt->src_ip)) {
        return 0;
    }
    /* Vérification de la présence de l'adresse IP source dans la liste noire (blacklist).*/
    if (ip_tree_contains(engine->blacklist, pkt->src_ip)) {
        alert.timestamp_ms = pkt->timestamp_ms;
        alert.src_ip       = pkt->src_ip;
        alert.dst_ip       = pkt->dst_ip;
        alert.severity     = ALERT_CRITICAL;
        strcpy(alert.rule_name, "Filtrage_Reputation_Blacklist");
        snprintf(alert.message, sizeof(alert.message),
                 "Paquet bloque : adresse IP source repertoriee dans l'ABR Blacklist");
        engine->on_alert(&alert, engine->alert_ctx);
        return 1;
    }
    for (i = 0; i < engine->rules_len; ++i) {
        if (engine->rules[i].enabled && engine->rules[i].check != NULL) {
            memset(&alert, 0, sizeof(Alert));
            if (engine->rules[i].check(pkt, session, &alert)) {
                engine->on_alert(&alert, engine->alert_ctx);
                ++alerts_triggered;

                if (alert.severity == ALERT_CRITICAL && engine->auto_ban_attackers) {
                    ip_tree_insert(&(engine->blacklist), pkt->src_ip, alert.rule_name);
                }
            }
        }
    }
    return alerts_triggered;
}
/* Libération des ressources allouées par le moteur de règles.*/
void engine_free(RulesEngine * engine) {
    int i;

    if (engine == NULL) {
        return;
    }
    ip_tree_free(engine->whitelist);
    ip_tree_free(engine->blacklist);
    engine->whitelist = NULL;
    engine->blacklist = NULL;

    if (engine->rules != NULL) {
        for (i = 0; i < engine->rules_len; ++i) {
            if (engine->rules[i].dl_handle != NULL) {
                dlclose(engine->rules[i].dl_handle);
            }
        }
        free(engine->rules);
        engine->rules = NULL;
    }
    engine->rules_len = 0;
    engine->rules_capacity = 0;
}
/* Fonction de vérification de la règle intégrée pour la détection de scans SYN.*/
int rule_check_syn_scan(const Packet * pkt, const SessionEntry * session, Alert * out_alert) {
    if (pkt == NULL || session == NULL || out_alert == NULL) {
        return 0;
    }
    if (pkt->protocol == PROTO_TCP && session->state == STATE_SYN_SENT) {
        if (session->syn_count >= 5) {
            out_alert->timestamp_ms = pkt->timestamp_ms;
            out_alert->src_ip       = pkt->src_ip;
            out_alert->dst_ip       = pkt->dst_ip;
            out_alert->severity     = ALERT_CRITICAL;
            strcpy(out_alert->rule_name, "Detection_SYN_Scan");
            snprintf(out_alert->message, sizeof(out_alert->message),
                     "Scan SYN detecte : %u paquets SYN sans ACK vers le port %u",
                     session->syn_count, pkt->dst_port);
            return 1;
        }
    }
    return 0;
}
/* Fonction de vérification de la règle intégrée pour la détection d'attaques volumétriques (DoS).*/
int rule_check_dos_volume(const Packet * pkt, const SessionEntry * session, Alert * out_alert) {
    if (pkt == NULL || session == NULL || out_alert == NULL) {
        return 0;
    }
    if (session->total_bytes >= 500000UL) {
        out_alert->timestamp_ms = pkt->timestamp_ms;
        out_alert->src_ip       = pkt->src_ip;
        out_alert->dst_ip       = pkt->dst_ip;
        out_alert->severity     = ALERT_WARNING;
        strcpy(out_alert->rule_name, "Detection_Pic_Volumetrique_DoS");
        snprintf(out_alert->message, sizeof(out_alert->message),
                 "Anomalie volumetrique : %lu octets recus (%u paquets)",
                 session->total_bytes, session->packet_count);
        return 1;
    }
    return 0;
}
/* Fonction de vérification de la règle intégrée pour la détection d'adresses IP suspectes en .10.*/
int rule_check_dot10_ip(const Packet * pkt, const SessionEntry * session, Alert * out_alert) {
    unsigned char first_octet;
    unsigned char last_octet;

    (void)session;
    if (pkt == NULL || out_alert == NULL) {
        return 0;
    }
    first_octet = (unsigned char)((pkt->src_ip >> 24) & 0xFF);
    last_octet  = (unsigned char)(pkt->src_ip & 0xFF);

    if (last_octet == 10 || first_octet == 10) {
        out_alert->timestamp_ms = pkt->timestamp_ms;
        out_alert->src_ip       = pkt->src_ip;
        out_alert->dst_ip       = pkt->dst_ip;
        out_alert->severity     = ALERT_WARNING;
        strcpy(out_alert->rule_name, "Heuristique_IP_Point10");
        snprintf(out_alert->message, sizeof(out_alert->message),
                 "IP suspecte detectee par masque binaire (1er octet=%u, dernier octet=%u)",
                 (unsigned int)first_octet, (unsigned int)last_octet);
        return 1;
    }
    return 0;
}
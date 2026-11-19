/*
 * alert.c
 * -------
 * Implementation du module de gestion des alertes.
 * Ce fichier convertit les niveaux de gravite en texte, affiche une alerte
 * lisible dans le terminal et journalise son contenu au format JSON Lines.
 */

/* Entrées/sorties standard, horloge Unix et declarations du projet. */
#include <stdio.h>
#include <time.h>
#include "alert.h"
#include "state_table.h"

static const char *alert_severity_text(AlertSeverity severity)
{
    /* Le code numerique est traduit en etiquette lisible pour les journaux. */
    switch (severity) {
        case ALERT_INFO:
            return "INFO";

        case ALERT_WARNING:
            return "WARNING";

        case ALERT_CRITICAL:
            return "CRITICAL";

        default:
            return "UNKNOWN";
    }
}

void alert_print(Alert alert)
{
    /* Tampons destines a la date, puis aux adresses IP converties en texte. */
    char date[32];
    char src[16];
    char dst[16];

    time_t seconds = (time_t)(alert.timestamp_ms / 1000UL);
    struct tm *local_time = localtime(&seconds);

    if (local_time == NULL) {
        return;
    }

    if (strftime(date, sizeof(date),
                 "%Y-%m-%d %H:%M:%S", local_time) == 0) {
        return;
    }

    ip_to_string(alert.src_ip, src, sizeof(src));
    ip_to_string(alert.dst_ip, dst, sizeof(dst));

    printf("[%s] [%s] %s\n",
           date, alert_severity_text(alert.severity),
           alert.message);

    printf("Regle : %s\n", alert.rule_name);
    printf("Source : %s\n", src);
    printf("Destination : %s\n", dst);
}

static void json_write_string(FILE *file, const char text[])
{
    /* Echappe les caracteres speciaux afin de produire une chaine JSON valide. */
    int i;
    unsigned char character;

    fputc('"', file);

    for (i = 0; text[i] != '\0'; i++) {
        character = (unsigned char)text[i];

        if (character == '"') {
            fputs("\\\"", file);
        }
        else if (character == '\\') {
            fputs("\\\\", file);
        }
        else if (character < 32) {
            fprintf(file, "\\u%04x", (unsigned int)character);
        }
        else {
            fputc(character, file);
        }
    }

    fputc('"', file);
}

int alert_log_json(Alert alert, const char filepath[])
{
    /* Ouvre le journal en ajout pour conserver les alertes precedentes. */
    FILE *file;
    int success = 1;

    char src[16];
    char dst[16];

    if (filepath == NULL) {
        return 0;
    }

    file = fopen(filepath, "a");

    if (file == NULL) {
        return 0;
    }

    ip_to_string(alert.src_ip, src, sizeof(src));
    ip_to_string(alert.dst_ip, dst, sizeof(dst));

    fprintf(file, "{\"timestamp_ms\":%lu", alert.timestamp_ms);

    fprintf(file, ",\"severity\":");
    json_write_string(file, alert_severity_text(alert.severity));

    fprintf(file, ",\"src_ip\":");
    json_write_string(file, src);

    fprintf(file, ",\"dst_ip\":");
    json_write_string(file, dst);

    fprintf(file, ",\"rule_name\":");
    json_write_string(file, alert.rule_name);

    fprintf(file, ",\"message\":");
    json_write_string(file, alert.message);

    fprintf(file, "}\n");

    if (ferror(file)) {
        success = 0;
    }

    if (fclose(file) != 0) {
        success = 0;
    }

    return success;
}

void alert_callback_json(const Alert *alert, void *user_context)
{
    /* user_context contient facultativement le chemin du fichier JSON. */
    const char *filepath = (const char *)user_context;

    if (alert == NULL) {
        return;
    }

    alert_print(*alert);

    if (filepath != NULL) {
        if (alert_log_json(*alert, filepath) == 0) {
            fprintf(stderr, "Erreur d'enregistrement de l'alerte.\n");
        }
    }
}

#include <stdio.h>
#include "alert.h"

void alert_print(Alert alert)
{
    char date[32];
    struct tm *local_time;

    /* Ces deux fonctions transforment la date en texte lisible. */
    local_time = localtime(&alert.timestamp);
    if (local_time == NULL) {
        return;
    }

    if (strftime(date, sizeof(date), "%Y-%m-%d %H:%M:%S", local_time) == 0) {
        return;
    }

    printf("[%s] [%s] %s | Source : %s | Destination : %s\n",
           date, alert.severity, alert.message, alert.src_ip, alert.dst_ip);
}

/* Fonction interne : ecrit du texte entre guillemets au format JSON. */
static void json_write_string(FILE *file, const char text[])
{
    int i;
    unsigned char character;

    fputc('"', file);

    for (i = 0; text[i] != '\0'; i++) {
        character = (unsigned char)text[i];

        if (character == '"') {
            fputs("\\\"", file);
        } else if (character == '\\') {
            fputs("\\\\", file);
        } else if (character < 32) {
            /* En JSON, les caracteres de controle doivent etre codes. */
            fprintf(file, "\\u%04x", (unsigned int)character);
        } else {
            fputc(character, file);
        }
    }

    fputc('"', file);
}

int alert_log_json(Alert alert, const char filepath[])
{
    FILE *file;
    int success = 1;

    if (filepath == NULL) {
        return 0;
    }

    /* Le mode "a" ajoute les alertes sans effacer les precedentes. */
    file = fopen(filepath, "a");
    if (file == NULL) {
        return 0;
    }

    /* Une alerte est un objet JSON, ecrit sur une seule ligne. */
    fprintf(file, "{\"timestamp\":%lld", (long long)alert.timestamp);

    fprintf(file, ",\"severity\":");
    json_write_string(file, alert.severity);

    fprintf(file, ",\"src_ip\":");
    json_write_string(file, alert.src_ip);

    fprintf(file, ",\"dst_ip\":");
    json_write_string(file, alert.dst_ip);

    fprintf(file, ",\"message\":");
    json_write_string(file, alert.message);

    fprintf(file, "}\n");

    /* On verifie les ecritures, puis on ferme toujours le fichier. */
    if (ferror(file)) {
        success = 0;
    }
    if (fclose(file) != 0) {
        success = 0;
    }

    return success;
}

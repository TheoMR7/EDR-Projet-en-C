#include "../include/state_table.h"
#include <string.h>
/* Fonction de conversion d'une adresse IP en format binaire (4 octets) à partir de 4 octets séparés.*/
unsigned int ip_from_bytes(unsigned char a, unsigned char b, unsigned char c, unsigned char d) {
    return ((unsigned int)a << 24) |
           ((unsigned int)b << 16) |
           ((unsigned int)c << 8)  |
           ((unsigned int)d);
}
/* Fonction de conversion d'une chaîne de caractères représentant une adresse IP en format binaire (4 octets).*/
int ip_from_string(const char * ip_str, unsigned int * out_ip) {
    unsigned int a, b, c, d;

    if (ip_str == NULL || out_ip == NULL) {
        return 0;
    }
    if (sscanf(ip_str, "%u.%u.%u.%u", &a, &b, &c, &d) != 4) {
        return 0;
    }
    if (a > 255 || b > 255 || c > 255 || d > 255) {
        return 0;
    }
    *out_ip = ip_from_bytes((unsigned char)a, (unsigned char)b,
                            (unsigned char)c, (unsigned char)d);
    return 1;
}
/* Fonction de conversion d'une adresse IP en format binaire (4 octets) vers une chaîne de caractères.*/
void ip_to_string(unsigned int ip, char * buffer, int buffer_len) {
    unsigned char a, b, c, d;

    if (buffer == NULL || buffer_len < 16) {
        return;
    }
    a = (unsigned char)((ip >> 24) & 0xFF);
    b = (unsigned char)((ip >> 16) & 0xFF);
    c = (unsigned char)((ip >> 8)  & 0xFF);
    d = (unsigned char)(ip & 0xFF);

    snprintf(buffer, (size_t)buffer_len, "%u.%u.%u.%u", a, b, c, d);
}
/* Fonction d'insertion d'une adresse IP dans l'arbre binaire de recherche (ABR) avec un commentaire associé.*/
int ip_tree_insert(IpNode ** root, unsigned int ip, const char * comment) {
    IpNode * new_node = NULL;

    if (root == NULL) {
        return 0;
    }
    if (*root == NULL) {
        new_node = (IpNode *)malloc(sizeof(IpNode));
        if (new_node == NULL) {
            fprintf(stderr, "Erreur allocation memoire dans ip_tree_insert\n");
            return 0;
        }
        new_node->ip = ip;
        if (comment != NULL) {
            strncpy(new_node->comment, comment, sizeof(new_node->comment) - 1); /* Copie sécurisée anti-buffer overflow */
            new_node->comment[sizeof(new_node->comment) - 1] = '\0';
        } else {
            strcpy(new_node->comment, "N/A");
        }
        new_node->left  = NULL;
        new_node->right = NULL;
        *root = new_node;
        return 1;
    }
/* Si l'adresse IP existe déjà, on met à jour le commentaire.*/
    if (ip == (*root)->ip) {
        if (comment != NULL) {
            strncpy((*root)->comment, comment, sizeof((*root)->comment) - 1);
            (*root)->comment[sizeof((*root)->comment) - 1] = '\0';
        }
        return 1;
    }
    if (ip < (*root)->ip) {
        return ip_tree_insert(&((*root)->left), ip, comment);
    } else {
        return ip_tree_insert(&((*root)->right), ip, comment);
    }
}
/* Fonction de vérification de la présence d'une adresse IP dans l'arbre binaire de recherche (ABR).*/
int ip_tree_contains(const IpNode * root, unsigned int ip) {
    const IpNode * current = root;
    while (current != NULL) {
        if (ip == current->ip) {
            return 1;
        } else if (ip < current->ip) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    return 0;
}
/* Fonction de suppression d'une adresse IP de l'arbre binaire de recherche (ABR).*/
static IpNode * find_min_node(IpNode * node) {
    IpNode * current = node;
    while (current != NULL && current->left != NULL) {
        current = current->left;
    }
    return current;
}
/* Fonction de suppression d'une adresse IP de l'arbre binaire de recherche (ABR).*/
int ip_tree_remove(IpNode ** root, unsigned int ip) {
    IpNode * temp = NULL;

    if (root == NULL || *root == NULL) {
        return 0;
    }
    if (ip < (*root)->ip) {
        return ip_tree_remove(&((*root)->left), ip);
    } else if (ip > (*root)->ip) {
        return ip_tree_remove(&((*root)->right), ip);
    } else {
        if ((*root)->left == NULL) {
            temp = (*root)->right;
            free(*root);
            *root = temp; /*Met à jour le pointeur du parent pour pointer vers le sous-arbre droit*/
            return 1;
        } else if ((*root)->right == NULL) {
            temp = (*root)->left;
            free(*root);
            *root = temp;
            return 1;
        }
        temp = find_min_node((*root)->right);
        (*root)->ip = temp->ip;
        snprintf((*root)->comment,
            sizeof((*root)->comment),
            "%s",
            temp->comment);
        (*root)->comment[sizeof((*root)->comment) - 1] = '\0';
        return ip_tree_remove(&((*root)->right), temp->ip);
    }
}
/* Fonction d'affichage des adresses IP dans l'arbre binaire de recherche (ABR) par ordre croissant.*/
void ip_tree_print(const IpNode * root) {
    char ip_str[16];

    if (root == NULL) {
        return;
    }
    ip_tree_print(root->left);
    ip_to_string(root->ip, ip_str, sizeof(ip_str));
    printf("   - %-15s (%s)\n", ip_str, root->comment);
    ip_tree_print(root->right);
}

void ip_tree_free(IpNode * root) {
    if (root == NULL) {
        return;
    }
    ip_tree_free(root->left);
    ip_tree_free(root->right);
    free(root);
}
/* Fonction de chargement des adresses IP à partir d'un fichier. */
int ip_tree_load_file(IpNode ** root, const char * filepath) {
    FILE * file = NULL;
    char ip_str[32];
    char comment[64];
    unsigned int ip_val;
    int count = 0;

    if (root == NULL || filepath == NULL) {
        return -1;
    }
    file = fopen(filepath, "r");
    if (file == NULL) {
        return -1;
    }
    while (fscanf(file, "%31s %63s", ip_str, comment) == 2) {
        if (ip_from_string(ip_str, &ip_val)) {
            ip_tree_insert(root, ip_val, comment);
            ++count;
        }
    }
    fclose(file);
    return count;
}
/* Fonction de sauvegarde des adresses IP dans un fichier. */
static void save_node_recursive(const IpNode * node, FILE * file) {
    char ip_str[16];

    if (node == NULL) {
        return;
    }
    save_node_recursive(node->left, file);
    ip_to_string(node->ip, ip_str, sizeof(ip_str));
    fprintf(file, "%s %s\n", ip_str, node->comment);
    save_node_recursive(node->right, file);
}
/* Fonction de sauvegarde des adresses IP dans un fichier. */
int ip_tree_save_file(const IpNode * root, const char * filepath) {
    FILE * file = NULL;

    if (filepath == NULL) {
        return 0;
    }
    file = fopen(filepath, "w");
    if (file == NULL) {
        return 0;
    }
    save_node_recursive(root, file);
    fclose(file);
    return 1;
}
/* Initialisation de la table d'état des sessions réseau.*/
void session_table_init(SessionTable * table) {
    int i;

    if (table == NULL) {
        return;
    }
    for (i = 0; i < HASH_TABLE_SIZE; ++i) {
        table->buckets[i] = NULL;
    }
    table->active_sessions = 0;
    table->total_packets_processed = 0;
}
/* Fonction de hachage pour générer un index dans la table d'état des sessions réseau.*/
unsigned int session_hash(unsigned int src_ip, unsigned int dst_ip,
                          unsigned short src_port, unsigned short dst_port,
                          ProtocolType proto) {
    unsigned int hash = 2166136261U;

    hash ^= src_ip;
    hash *= 16777619U;
    hash ^= dst_ip;
    hash *= 16777619U;
    hash ^= (unsigned int)((src_port << 16) | dst_port);
    hash ^= (unsigned int)proto;

    return hash % HASH_TABLE_SIZE;
}
/* Fonction de mise à jour de la table d'état des sessions réseau avec un nouveau paquet.*/
SessionEntry * session_table_update(SessionTable * table, const Packet * pkt) {
    unsigned int index;
    SessionEntry * current = NULL;
    SessionEntry * new_entry = NULL;

    if (table == NULL || pkt == NULL) {
        return NULL;
    }
/* Incrémente le compteur total de paquets traités.*/
    ++(table->total_packets_processed);
    index = session_hash(pkt->src_ip, pkt->dst_ip, pkt->src_port, pkt->dst_port, pkt->protocol);
    current = table->buckets[index];

    while (current != NULL) {
        if (current->src_ip == pkt->src_ip &&
            current->dst_ip == pkt->dst_ip &&
            current->src_port == pkt->src_port &&
            current->dst_port == pkt->dst_port &&
            current->protocol == pkt->protocol) {

            ++(current->packet_count);
            current->total_bytes += pkt->payload_size;
            current->last_seen_ms = pkt->timestamp_ms;

            if (pkt->protocol == PROTO_TCP) {
                if ((pkt->tcp_flags & TCP_FLAG_SYN) && !(pkt->tcp_flags & TCP_FLAG_ACK)) {
                    ++(current->syn_count);
                }
                if (pkt->tcp_flags & TCP_FLAG_ACK) {
                    current->state = STATE_ESTABLISHED;
                }
                if ((pkt->tcp_flags & TCP_FLAG_FIN) || (pkt->tcp_flags & TCP_FLAG_RST)) {
                    current->state = STATE_CLOSED;
                }
            }
            return current;
        }
        current = current->next;
    }

    new_entry = (SessionEntry *)malloc(sizeof(SessionEntry));
    if (new_entry == NULL) {
        fprintf(stderr, "Erreur allocation memoire dans session_table_update\n");
        return NULL;
    }
/* Initialisation des champs de la nouvelle entrée de session.*/
    new_entry->src_ip        = pkt->src_ip;
    new_entry->dst_ip        = pkt->dst_ip;
    new_entry->src_port      = pkt->src_port;
    new_entry->dst_port      = pkt->dst_port;
    new_entry->protocol      = pkt->protocol;
    new_entry->packet_count  = 1;
    new_entry->total_bytes   = pkt->payload_size;
    new_entry->first_seen_ms = pkt->timestamp_ms;
    new_entry->last_seen_ms  = pkt->timestamp_ms;

    if (pkt->protocol == PROTO_TCP && (pkt->tcp_flags & TCP_FLAG_SYN)) {
        new_entry->state     = STATE_SYN_SENT;
        new_entry->syn_count = 1;
    } else {
        new_entry->state     = STATE_NEW;
        new_entry->syn_count = 0;
    }

    new_entry->next = table->buckets[index];
    table->buckets[index] = new_entry;
    ++(table->active_sessions);

    return new_entry;
}
/* Fonction de nettoyage des sessions expirées dans la table d'état des sessions réseau.*/
unsigned int session_table_cleanup_expired(SessionTable * table,
                                           unsigned long current_time_ms,
                                           unsigned long timeout_ms) {
    int i;
    unsigned int removed_count = 0;
    SessionEntry * current = NULL;
    SessionEntry * prev = NULL;
    SessionEntry * to_delete = NULL;

    if (table == NULL) {
        return 0;
    }

    for (i = 0; i < HASH_TABLE_SIZE; ++i) {
        current = table->buckets[i];
        prev = NULL;

        while (current != NULL) {
            if ((current_time_ms >= current->last_seen_ms) &&
                ((current_time_ms - current->last_seen_ms) > timeout_ms)) {

                to_delete = current;
                if (prev == NULL) {
                    table->buckets[i] = current->next;
                    current = table->buckets[i];
                } else {
                    prev->next = current->next;
                    current = prev->next;
                }
                free(to_delete);
                --(table->active_sessions);
                ++removed_count;
            } else {
                prev = current;
                current = current->next;
            }
        }
    }
    return removed_count;
}
/* Fonction d'affichage d'un résumé de la table d'état des sessions réseau.*/
void session_table_print_summary(const SessionTable * table) {
    int i;
    const SessionEntry * current = NULL;
    char src_str[16];
    char dst_str[16];

    if (table == NULL) {
        return;
    }
    /* Affiche un en-tête pour le résumé de la table d'état des sessions réseau.*/
    printf("--- TABLE D'ETAT DES CONNEXIONS (%u sessions actives | %lu paquets) ---\n",
           table->active_sessions, table->total_packets_processed);
    printf(" PROTO | SOURCE                | DESTINATION           | PAQUETS | OCTETS  | SYN\n");
    printf("-------+-----------------------+-----------------------+---------+---------+-----\n");

    for (i = 0; i < HASH_TABLE_SIZE; ++i) {
        current = table->buckets[i];
        while (current != NULL) {
            ip_to_string(current->src_ip, src_str, sizeof(src_str));
            ip_to_string(current->dst_ip, dst_str, sizeof(dst_str));
            printf(" %-5d | %-15s:%-5u | %-15s:%-5u | %-7u | %-7lu | %-3u\n",
                   (int)current->protocol,
                   src_str, current->src_port,
                   dst_str, current->dst_port,
                   current->packet_count,
                   current->total_bytes,
                   current->syn_count);
            current = current->next;
        }
    }
}
/* Fonction de libération de la mémoire allouée pour la table d'état des sessions réseau.*/
void session_table_free(SessionTable * table) {
    int i;
    SessionEntry * current = NULL;
    SessionEntry * next_node = NULL;

    if (table == NULL) {
        return;
    }
    for (i = 0; i < HASH_TABLE_SIZE; ++i) {
        current = table->buckets[i];
        while (current != NULL) {
            next_node = current->next;
            free(current);
            current = next_node;
        }
        table->buckets[i] = NULL;
    }
    table->active_sessions = 0;
}
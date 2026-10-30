#include "state_table.h"
#include <stdio.h>
#include <arpa/inet.h>

unsigned int ip_from_bytes(unsigned char a, unsigned char b,
                           unsigned char c, unsigned char d)
{
    return ((unsigned int)a << 24) |
           ((unsigned int)b << 16) |
           ((unsigned int)c << 8) |
           (unsigned int)d;
}

int ip_from_string(const char *ip_str, unsigned int *out_ip)
{
    struct in_addr address;

    if (ip_str == NULL || out_ip == NULL) {
        return 0;
    }

    /* Fonction Linux : verifie et convertit une adresse IPv4. */
    if (inet_pton(AF_INET, ip_str, &address) != 1) {
        return 0;
    }

    /* Retrouve le meme ordre d'octets que ip_from_bytes. */
    *out_ip = ntohl(address.s_addr);
    return 1;
}

void ip_to_string(unsigned int ip, char *buffer, int buffer_len)
{
    unsigned char a, b, c, d;

    if (buffer == NULL || buffer_len < 16) {
        return;
    }

    a = (unsigned char)((ip >> 24) & 0xFF);
    b = (unsigned char)((ip >> 16) & 0xFF);
    c = (unsigned char)((ip >> 8) & 0xFF);
    d = (unsigned char)(ip & 0xFF);

    snprintf(buffer, (size_t)buffer_len, "%u.%u.%u.%u",
             (unsigned int)a, (unsigned int)b,
             (unsigned int)c, (unsigned int)d);
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <sys/socket.h>

#define BUFFER_SIZE 65536
// #define TCP_FLAG_FIN 0x01
// #define TCP_FLAG_SYN 0x02
// #define TCP_FLAG_RST 0x04
// #define TCP_FLAG_PSH 0x08
// #define TCP_FLAG_ACK 0x10
// #define TCP_FLAG_URG 0x20

typedef enum ProtocolType ProtocolType;
enum ProtocolType {
    PROTO_UNKNOWN = 0,
    PROTO_ICMP    = 1,
    PROTO_TCP     = 6,
    PROTO_UDP     = 17
};

typedef struct Packet Packet;
struct Packet {
    unsigned int   src_ip;
    unsigned int   dst_ip;
    unsigned short src_port;
    unsigned short dst_port;
    ProtocolType   protocol;
    // unsigned char  tcp_flags;
    // unsigned int   payload_size;
    // unsigned long  timestamp_ms;
};

/* Fonction de conversion d'une adresse IP en format binaire (4 octets) à partir de 4 octets séparés.*/
unsigned int ip_from_bytes(unsigned char a, unsigned char b, unsigned char c, unsigned char d) {
    return ((unsigned int)a << 24) |
           ((unsigned int)b << 16) |
           ((unsigned int)c << 8)  |
           ((unsigned int)d);
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

/* Fonction qui permet d'afficher les octets d'un paquet en ligne de 16 afin d'améliorer la lisibilité*/
void hex_dump(const unsigned char *buf, ssize_t len) {
    for (ssize_t i = 0; i < len; i++){
        printf("%02x ", buf[i]);
        if ((i + 1) % 16 == 0) {
            printf("\n");
        }
    }
    printf("\n");
}

int main(void) {
	int socket_fd;
	ssize_t data_size;
    unsigned char buffer[BUFFER_SIZE];
	unsigned int ethertype;
	int IHL_offset;
	char s[16],d[16];

    socket_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL)); /*ouvre le socket*/
    if (socket_fd < 0){
        perror("socket");
        return EXIT_FAILURE;
    }

    while (1){
        data_size = recvfrom(socket_fd, buffer, BUFFER_SIZE, 0, NULL, NULL); /*recoit l'information du packet depuis socket_fd, le mets dans buffer et rend la taille en bytes dans data_size*/
        if (data_size < 0){
            perror("recvfrom");
            break;
        }
		if (data_size < 34){ /*si plus petit que la taille d'un paquet ethernet mini, on veut uniquement des packets ethernet*/
			continue;
		}
		ethertype = (buffer[12] << 8) | buffer[13];
		if (ethertype != 0x0800){ /*si pas IPv4*/
			continue;
		}
		if (buffer[23] != 6 && buffer[23] != 17){ /*si protocol est ni TCP, ni UDP*/
			continue;
		}
		IHL_offset = 14 + ((buffer[14] & 0x0F) * 4); /*décalage nécessaire pour atteindre les informations TCP/UDP*/
        if (IHL_offset + 4 > data_size){ /*si le décalage plus 4 est plus grand que le nombre de bytes on va récupérer des informations erronées*/
			continue;
		}
		Packet capture;
		capture.src_ip = ip_from_bytes(buffer[26],buffer[27],buffer[28],buffer[29]);
		capture.dst_ip = ip_from_bytes(buffer[30],buffer[31],buffer[32],buffer[33]);
		capture.src_port = (buffer[IHL_offset] << 8 | buffer[IHL_offset + 1]);
		capture.dst_port = (buffer[IHL_offset + 2] << 8 | buffer[IHL_offset + 3]);
		switch (buffer[23])
		{
			case 1: capture.protocol = PROTO_ICMP; break;
			case 6: capture.protocol = PROTO_TCP; break;
			case 17: capture.protocol = PROTO_UDP; break;		
			default: capture.protocol = PROTO_UNKNOWN; break;
		}
		printf("%zd bytes dans le paquet\n", data_size); /*nombre de bytes total*/
        /*printf("ethertype = 0x%04x\n", ethertype);si IPv4*/
		printf("protocol = %d\n", (int)capture.protocol);/*type de protocol 6 ou 17 actuellement*/
		printf("Port Source = %d | Port Destination = %d\n", capture.src_port, capture.dst_port);/*ports source et destination*/
		/*printf("src_IP = %d.%d.%d.%d ; dst_IP = %d.%d.%d.%d\n",buffer[26],buffer[27],buffer[28],buffer[29],buffer[30],buffer[31],buffer[32],buffer[33]);adresses IP source et destination*/
		ip_to_string(capture.src_ip,s,sizeof(s));
		ip_to_string(capture.dst_ip,d,sizeof(d));
		printf("Source = %s | Destination = %s\n", s,d);
		hex_dump(buffer, data_size);/*le packet en byte par ligne de 16*/
		printf("\n");
    }

    close(socket_fd);
    return EXIT_SUCCESS;
}
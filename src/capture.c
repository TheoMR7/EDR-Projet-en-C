#include <capture.h>

static void hex_dump(const unsigned char *buf, ssize_t len) {
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
	unsigned int IHL_offset;
	int src_port, dst_port;

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
		src_port = (buffer[IHL_offset] << 8 | buffer[IHL_offset + 1]);
		dst_port = (buffer[IHL_offset + 2] << 8 | buffer[IHL_offset + 3]);
		printf("%zd bytes dans le paquet\n", data_size); /*nombre de bytes total*/
        printf("ethertype = 0x%04x\n", ethertype);/*si IPv4*/
		printf("protocol = %d\n", buffer[23]);/*type de protocol 6 ou 17 actuellement*/
		printf("src_port = %d ; dst_port = %d\n", src_port, dst_port);/*ports source et destination*/
		printf("src_IP = %d.%d.%d.%d ; dst_IP = %d.%d.%d.%d\n",buffer[26],buffer[27],buffer[28],buffer[29],buffer[30],buffer[31],buffer[32],buffer[33]);/*adresses IP source et destination*/
		hex_dump(buffer, data_size);/*le packet en byte par ligne de 16*/
		printf("\n");
    }

    close(socket_fd);
    return EXIT_SUCCESS;
}
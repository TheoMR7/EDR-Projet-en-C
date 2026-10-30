#ifndef PACKET_H
#define PACKET_H

typedef enum ProtocolType ProtocolType;
enum ProtocolType {
    PROTOCOL_TCP = 6,
    PROTOCOL_UDP = 17,
    PROTOCOL_ICMP = 1,
    PROTOCOL_UNKNOWN = 0
};

#define TCP_FLAG_FIN 0x01
#define TCP_FLAG_SYN 0x02
#define TCP_FLAG_RST 0x04
#define TCP_FLAG_PSH 0x08
#define TCP_FLAG_ACK 0x10
#define TCP_FLAG_URG 0x20

typedef struct Packet Packet;
struct Packet {
    unsigned int src_ip;
    unsigned int dst_ip;
    unsigned short src_port;
    unsigned short dst_port;
    ProtocolType protocol;
    unsigned char tcp_flags;
    unsigned int payload_size;
    unsigned long timestamp_ms;
};

#endif

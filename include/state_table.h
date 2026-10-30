#ifndef STATE_TABLE_H
#define STATE_TABLE_H

#include "packet.h"
#define HASH_TABLE_SIZE 1021

typedef struct IpNode IpNode;
struct IpNode{
    unsigned int    ip;
    char    comment[64];
    IpNode *    left;
    IpNode *    right;
};
unsigned int ip_from_bytes(unsigned char a, unsigned char b, unsigned char c, unsigned char d);
int ip_from_string(const char * ip_str, unsigned int * out_ip);
void    ip_to_string(unsigned int ip, char * buffer, int buffer_len);

int ip_tree_insert(IpNode ** root, unsigned int ip, const char * comment);
int ip_tree_contains(const IpNode * root, unsigned int ip);
int ip_tree_remove(IpNode ** root, unsigned int ip);
void ip_tree_print(const IpNode * root);
void ip_tree_free(IpNode * root);

int ip_tree_load_file(IpNode ** root, const char * filepath);
int ip_tree_save_file(const IpNode * root, const char * filepath);

typedef enum SessionState SessionState;
enum SessionState{
    STATE_NEW   =0,
    STATE_SYN_SENT  =1,
    STATE_ESTABLISHED   =2,
    STATE_CLOSED    =3
};
typedef struct SessionEntry SessionEntry;
struct SessionEntry{
    unsigned int    src_ip;
    unsigned int    dst_ip;
    unsigned short  src_port;
    unsigned short  dst_port;
    ProtocolType    protocol;
    SessionState    state;
    unsigned int    packet_count;
    unsigned long   total_bytes;
    unsigned int    syn_count;
    unsigned long   first_seen_ms;
    unsigned long   last_seen_ms;
    SessionEntry * next;
};
typedef struct SessionTable SessionTable;
struct  SessionTable
{
    SessionEntry * buckets[HASH_TABLE_SIZE];
    unsigned int    active_sessions;
    unsigned long   total_packets_processed; 
};

void    session_table_init(SessionTable * table);
unsigned int session_hash(unsigned int src_ip, unsigned int dst_ip, unsigned short src_port, unsigned short dst_port, ProtocolType proto);
void session_table_print_summary(const SessionTable * table);
void session_table_free(SessionTable * table);

#endif

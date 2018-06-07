#ifndef IP_HEADER_H
#define IP_HEADER_H
typedef struct {
    unsigned char  hdr_len:4;
    unsigned char  version:4;
    unsigned char  tos;
    unsigned short total_len;
    unsigned short identifier;
    unsigned short frag_and_flags;
    unsigned char  ttl;
    unsigned char  protocol;
    unsigned short checksum;
    unsigned long  sourceIP;
    unsigned long  destIP;
} IP_HEADER;
#endif // IP_HEADER_H

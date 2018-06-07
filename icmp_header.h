#ifndef ICMP_HEADER_H
#define ICMP_HEADER_H
typedef struct {
    BYTE    type;
    BYTE    code;
    USHORT  cksum;
    USHORT  id;
    USHORT  seq;
} ICMP_HEADER;

const BYTE ICMP_ECHO_REQUEST = 8;
const BYTE ICMP_ECHO_REPLY  = 0;
const BYTE ICMP_TIMEOUT   = 11;
const DWORD DEF_ICMP_TIMEOUT = 3000;
const int DEF_ICMP_DATA_SIZE = 32;
const int MAX_ICMP_PACKET_SIZE = 1024;

#endif // ICMP_HEADER_H

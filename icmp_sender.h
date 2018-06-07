#ifndef ICMP_SENDER_H
#define ICMP_SENDER_H

#include <QObject>
#include <WinSock2.h>
#include <WS2tcpip.h>

typedef struct {
    USHORT seq_no;
    DWORD round_trip_time;
    in_addr ip_addr;
    BYTE type;
    int error_code;
} DECODE_RESULT;

class icmp_sender {
public:
    explicit icmp_sender(WSADATA wsa);
    DECODE_RESULT send_packet(QString addr, int ttl, int seqno);
    WSADATA wsa;

    SOCKET raw_socket;
private:
    u_long ip_conversion(QString addr);
    int create_socket();
    ushort calc_checksum(ushort* buf, int size);
    BOOL decode_icmp(char* buf, int packet_size, DECODE_RESULT& decode_result);
};

#endif // ICMP_SENDER_H

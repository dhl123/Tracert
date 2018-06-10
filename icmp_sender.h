#ifndef ICMP_SENDER_H
#define ICMP_SENDER_H

#include <QObject>
#include <WinSock2.h>
#include <WS2tcpip.h>

const int SENDER_SUCCESS = 0;
const int SENDER_INVALID_HOST = -1;

const int SENDER_SOCKET_INVALID = -100;
const int SENDER_SOCKET_OPTFAIL = -101;

const int SENDER_SEND_HOST_UNREACH = -200;
const int SENDER_SEND_TIMEDOUT = -201;
const int SENDER_SEND_WSAERROR = -202;

const int SENDER_PARSE_WRONG_PACKET_LENGTH = -300;
const int SENDER_PARSE_UNKNOWN_TYPE = -301;
const int SENDER_PARSE_INVAILD_ID = -302;
const int SENDER_PARSE_FAILURE = -303;

const DWORD DEF_ICMP_TIMEOUT = 5000;

const BYTE ICMP_ECHO = 0;
const BYTE ICMP_ECHO_REQUEST = 8;
const BYTE ICMP_TTL_EXCEEDED = 11;

typedef struct {
    // Sequecen number
    USHORT seq_no;

    // Delay from sending to received.
    DWORD round_trip_time;

    // Source ip
    in_addr ip_addr;

    // Packet type, see icmp_header.h for more information.
    // Can be ICMP_ECHO_REPLY, ICMP_ECHO_REQUEST, ICMP_TIMEOUT
    BYTE type;

    // Error code, if any error happens.
    // see SENDER_ const series.
    int error_code;
} DECODE_RESULT;

class icmp_sender {
public:
    explicit icmp_sender(WSADATA wsa);
    DECODE_RESULT send_packet(QString ip, int ttl, int seqno);

private:
    WSADATA wsa;
};

#endif // ICMP_SENDER_H

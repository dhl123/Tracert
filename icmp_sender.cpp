#include "icmp_sender.h"
#include "icmp_header.h"
#include "ip_header.h"
#include <QDebug>

icmp_sender::icmp_sender(WSADATA wsa) {
    this->wsa = wsa;
    create_socket();
}

DECODE_RESULT icmp_sender::send_packet(QString addr, int ttl, int seqno) {
    // host resolve
    u_long dest_ip = this->ip_conversion(addr);

    DECODE_RESULT decode_result;
    decode_result.error_code = 0;

    if (dest_ip == -1) {
        decode_result.error_code = -1;
        return decode_result;
        //TODO: error handler
    }

    // data buffer
    sockaddr_in dest_sock_addr;
    ZeroMemory(&dest_sock_addr, sizeof(sockaddr_in));
    dest_sock_addr.sin_family = AF_INET;
    dest_sock_addr.sin_addr.s_addr = dest_ip;

    char icmp_send_buf[sizeof(ICMP_HEADER) + DEF_ICMP_DATA_SIZE];
    memset(icmp_send_buf, 0, sizeof(icmp_send_buf));

    char icmp_recv_buf[MAX_ICMP_PACKET_SIZE];
    memset(icmp_recv_buf, 0, sizeof(icmp_recv_buf));

    // init icmp header
    ICMP_HEADER* icmp_header = (ICMP_HEADER*)icmp_send_buf;
    icmp_header->type = ICMP_ECHO_REQUEST;
    icmp_header->code = 0;
    icmp_header->id = (USHORT)GetCurrentProcessId();

    memset(icmp_send_buf+sizeof(ICMP_HEADER), '0', DEF_ICMP_DATA_SIZE);

    icmp_header->cksum = 0;
    icmp_header->seq = htons(seqno);
    icmp_header->cksum = this->calc_checksum((USHORT*)icmp_send_buf, sizeof(ICMP_HEADER) + DEF_ICMP_DATA_SIZE);

    decode_result.seq_no = icmp_header->seq;
    decode_result.round_trip_time = GetTickCount();

    // init decode result
    setsockopt(this->raw_socket, IPPROTO_IP, IP_TTL, (char*)&ttl, sizeof(ttl));

    // send icmp
    if (sendto(this->raw_socket, icmp_send_buf, sizeof(icmp_send_buf), 0, (sockaddr*)&dest_sock_addr, sizeof(dest_sock_addr)) == SOCKET_ERROR) {
        // error handler
        if (WSAGetLastError() == WSAEHOSTUNREACH) {
            decode_result.error_code = WSAEHOSTUNREACH;
            closesocket(this->raw_socket);
            WSACleanup();
            return decode_result;
        }
    }

    // recv
    sockaddr_in from;
    int from_length = sizeof(from);
    int read_data_length = 0;

    while(true) {
        read_data_length = recvfrom(this->raw_socket, icmp_recv_buf, MAX_ICMP_PACKET_SIZE, 0, (sockaddr*)&from, &from_length);
        if (read_data_length != SOCKET_ERROR) {
            if (decode_icmp(icmp_recv_buf, read_data_length, decode_result)) {
                return decode_result;
            }
        } else if (WSAGetLastError() == WSAETIMEDOUT) {
            //TODO: ERROR HANDLER
            decode_result.error_code = WSAETIMEDOUT;
            return decode_result;
        } else {
            //TODO: ERROR HANDLER
            decode_result.error_code = WSAGetLastError();
            closesocket(this->raw_socket);
            WSACleanup();
            return decode_result;
        }
    }

    return decode_result;
}

u_long icmp_sender::ip_conversion(QString addr) {
    std::string str = addr.toStdString();
    const char* cstr_addr = str.c_str();

    u_long dest = inet_addr(cstr_addr);
    // conversion failed, maybe a domain
    if (dest == INADDR_NONE) {
        hostent* domain = gethostbyname(cstr_addr);
        // host by domain
        if (domain) {
            dest = (*(in_addr*)domain->h_addr).s_addr;
        } else {
            //TODO: ERROR HANDLER
            return -1; //TODO: USING CONST INSTEAD OF MAGIC NUMBER
        }
    }
    return dest;
}

int icmp_sender::create_socket() {
    SOCKET sock_raw = WSASocket(AF_INET, SOCK_RAW, IPPROTO_ICMP, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (sock_raw == INVALID_SOCKET) {
        //TODO: ERROR HANDLER
        WSACleanup();
        return -1;
    }
    this->raw_socket = sock_raw;

    int timeout = DEF_ICMP_TIMEOUT;

    if (setsockopt(sock_raw, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) == SOCKET_ERROR) {
        //TODO: ERROR HANDLER
        closesocket(sock_raw);
        WSACleanup();
        return -1; //TODO: USE CONST INSTEAD OF MAGIC NUMBER
    }
    return 0;
}

ushort icmp_sender::calc_checksum(ushort *buf, int size) {
    unsigned long cksum = 0;
    while (size>1) {
        cksum += *buf++;
        size -= sizeof(USHORT);
    }
    if (size) {
        cksum += *(UCHAR*)buf;
    }
    cksum = (cksum >> 16) + (cksum & 0xffff);
    cksum += (cksum >> 16);
    return (USHORT)(~cksum);
}

BOOL icmp_sender::decode_icmp(char *buf, int packet_size, DECODE_RESULT &decode_result) {
    IP_HEADER* ip_header = (IP_HEADER*) buf;
    int ip_header_length = ip_header->hdr_len * 4;
    if (packet_size < (int)(ip_header_length + sizeof(ICMP_HEADER))) {
        //TODO: ERROR HANDLER
        return FALSE;
    }

    ICMP_HEADER* icmp_header = (ICMP_HEADER*)(buf + ip_header_length);
    USHORT id, seq_no;
    if (icmp_header->type == ICMP_ECHO_REPLY) {
        id = icmp_header->id;
        seq_no = icmp_header->seq;
    } else if (icmp_header->type == ICMP_TIMEOUT) {
        char* inner_ip_header = buf + ip_header_length + sizeof(ICMP_HEADER);
        int inner_ip_header_length = ((IP_HEADER*)inner_ip_header)->hdr_len * 4;
        ICMP_HEADER* inner_icmp_header = (ICMP_HEADER*)(inner_ip_header + inner_ip_header_length);
        id = inner_icmp_header->id;
        seq_no = inner_icmp_header->seq;
    } else {
        //TODO: ERROR HANDLER
        return FALSE;
    }

    if (id != (USHORT)GetCurrentProcessId() || seq_no != decode_result.seq_no) {
        //TODO: ERROR HANDLER
        return FALSE;
    }

    if (icmp_header->type == ICMP_ECHO_REPLY || icmp_header->type == ICMP_TIMEOUT) {
        decode_result.ip_addr.s_addr = ip_header->sourceIP;
        decode_result.round_trip_time = GetTickCount() - decode_result.round_trip_time;
        decode_result.type = icmp_header->type;
        return TRUE;
    }
    //TODO: ERROR HANDLER
    return FALSE;
}

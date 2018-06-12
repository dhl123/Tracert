#include "icmp_sender.h"
#include "utils.h"
#include <QDebug>
#include <WinSock2.h>
#include <iphlpapi.h>
#include <icmpapi.h>

/* Create ICMP Sender.
 * Parameters:
 *      WSADATA wsa: WSAStartup returns, store Windows Sockets data
 * Example:
 *      WSADATA wsa;
 *      if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
 *          return -1;
 *      }
 *      icmp_sender* sender = new icmp_sender(wsa);
 */
icmp_sender::icmp_sender(WSADATA wsa) {
    this->wsa = wsa;
    // create_socket();
}
//icmp_sender::icmp_sender(WSADATA wsa,QString ip, int ttl, int seqno){
//    this->wsa = wsa;
//    this->ip = ip;
//    this->ttl = ttl;
//    this->seqno = seqno;
//}

/* Send ICMP Echo Request packet.
 * Parameters:
 *      QString ip: IP of target remote
 *      int ttl: Time to live
 *      int seqno: Sequence number
 * Returns:
 *      DECODE_RESULT: Parse result of response packet from remote
 * Example:
 *      DECODE_RESULT result = sender->send_packet(QString("202.101.226.69"), 30, 0);
 *      std::cout << result.round_trip_time;
 */
DECODE_RESULT icmp_sender::send_packet(QString ip, int ttl, int seqno) {
    // host resolve
    u_long dest_ip = address_conversion(ip);

    DECODE_RESULT decode_result;
    decode_result.error_code = SENDER_SUCCESS;
    decode_result.seq_no = seqno;

    if (dest_ip == -1) {
        decode_result.error_code = SENDER_INVALID_HOST;
        return decode_result;
    }

    HANDLE icmp;
    if ((icmp = IcmpCreateFile()) == INVALID_HANDLE_VALUE) {
        decode_result.error_code = SENDER_SOCKET_INVALID;
        return decode_result;
    }
    IP_OPTION_INFORMATION ip_option;
    ZeroMemory(&ip_option, sizeof(IP_OPTION_INFORMATION));
    ip_option.Ttl = ttl;
    char send_data[32];
    memset(send_data, '0', sizeof(send_data));
    char reply_buffer[sizeof(ICMP_ECHO_REPLY) + 32];
    PICMP_ECHO_REPLY eche_reply = (PICMP_ECHO_REPLY)reply_buffer;

    if (IcmpSendEcho(icmp, (IPAddr)dest_ip, send_data, sizeof(send_data), &ip_option,
                     reply_buffer, sizeof(reply_buffer), DEF_ICMP_TIMEOUT) != 0) {
        decode_result.round_trip_time = eche_reply->RoundTripTime;
        decode_result.ip_addr = *(in_addr*)&(eche_reply->Address);
        decode_result.type = ICMP_TTL_EXCEEDED;
        if ((unsigned long)eche_reply->Address == dest_ip) {
            decode_result.type = ICMP_ECHO;
        }
    } else {
        decode_result.round_trip_time = -1;
        decode_result.error_code = SENDER_SEND_TIMEDOUT;
    }

    IcmpCloseHandle(icmp);

    return decode_result;
}

//void icmp_sender::run()
//{
////    int i=1;
//    qDebug()<<"send packet ";
//    send_packet(ip,ttl,seqno);
//}

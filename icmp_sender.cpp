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

    HANDLE hIcmp;
    if ((hIcmp = IcmpCreateFile()) == INVALID_HANDLE_VALUE) {
        decode_result.error_code = SENDER_SOCKET_INVALID;
        return decode_result;
    }
    IP_OPTION_INFORMATION IpOption;
    ZeroMemory(&IpOption,sizeof(IP_OPTION_INFORMATION));
    IpOption.Ttl = ttl;
    char SendData[32];
    memset(SendData,'0',sizeof(SendData));
    char ReplyBuffer[sizeof(ICMP_ECHO_REPLY)+32];
    PICMP_ECHO_REPLY pEchoReply = (PICMP_ECHO_REPLY)ReplyBuffer;

    if(IcmpSendEcho(hIcmp,(IPAddr)dest_ip, SendData, sizeof(SendData), &IpOption,
                    ReplyBuffer, sizeof(ReplyBuffer), DEF_ICMP_TIMEOUT)!=0) {
        decode_result.round_trip_time = pEchoReply->RoundTripTime;
        decode_result.ip_addr = *(in_addr*)&(pEchoReply->Address);
        decode_result.type = ICMP_TIMEOUT;
        if((unsigned long)pEchoReply->Address==dest_ip) {
            decode_result.type = ICMP_ECHO;
        }
    } else {
        decode_result.round_trip_time = -1;
        decode_result.error_code = SENDER_SEND_TIMEDOUT;
    }

    IcmpCloseHandle(hIcmp);

    return decode_result;
}

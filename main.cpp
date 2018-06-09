#include <stdio.h>
#include <stdlib.h>
#define HAVE_REMOTE
#include <WinSock2.h>
#include <iphlpapi.h>
#include <icmpapi.h>
#include "icmp_sender.h"
#include "ip_locator.h"
#include "traceroute.h"
#include "utils.h"
#include <iostream>

#define TARGET "220.181.57.216"

int main() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        //TODO: ERROR HANDLER
        return -1;
    }
    icmp_sender sender(wsa);
    DECODE_RESULT result = sender.send_packet(QString(TARGET), 30, 0);
    std::cout << result.round_trip_time;
}

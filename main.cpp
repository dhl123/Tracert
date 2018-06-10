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
#include <QCoreApplication>
#include<qdebug.h>
#include<icmp_sender.h>
#include<traceroute.h>
#define TARGET "220.181.57.216"

int main(int argc,char** argv) {
    QCoreApplication a(argc,argv);
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        //TODO: ERROR HANDLER
        return -1;
    }
    icmp_sender* sender=new icmp_sender(wsa);
    /*ip_locator* locator=new ip_locator(nullptr);

    locator->send_request(QString("118.28.8.8"),0);
    return a.exec();*/
    traceroute* trace=new traceroute(nullptr,QString("202.101.226.69"),1);
    trace->start_trace();

}

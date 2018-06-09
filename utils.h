#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <WinSock2.h>
#include <WS2tcpip.h>

QString ip_to_hostname(QString ip);
u_long hostname_to_ip(QString hostname);
u_long address_conversion(QString address);

#endif // UTILS_H

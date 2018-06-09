#include <QString>
#include "utils.h"
#include <WinSock2.h>
#include <WS2tcpip.h>

/* Convert an IP address to a domain.
 * Parameters:
 *      QString ip: IP address, decimal dotted format.
 * Returns:
 *      QString: Domain of this IP address
 */
QString ip_to_hostname(QString ip) {
    struct addrinfo hints;
    struct addrinfo *res = 0;
    int status;

    hints.ai_family = AF_INET;

    std::string str = ip.toStdString();
    const char* cstr_ip = str.c_str();
    status = getaddrinfo(cstr_ip, 0, 0, &res);

    char host[512];
    status = getnameinfo(res->ai_addr, res->ai_addrlen, host, 512, 0, 0, 0);
    QString hostname = QString(host);

    freeaddrinfo(res);
    return hostname;
}


/* Convert a domain to an IP address.
 * Parameters:
 *      QString hostname: A domain of remote.
 * Returns:
 *      u_long: IP address in number format.
 */
u_long hostname_to_ip(QString hostname) {
    std::string str = hostname.toStdString();
    const char* cstr_hostname = str.c_str();

    hostent* domain = gethostbyname(cstr_hostname);
    // host by domain
    if (domain) {
        u_long dest = (*(in_addr*)domain->h_addr).s_addr;
        return dest;
    } else {
        //TODO: ERROR HANDLER
        return -1;
    }
}

/* Convert a domain or a IP address into IP address in number format.
 * Parameters:
 *      QString address: A domain or IP address
 * Returns:
 *      u_long: IP address in number format.
 * See also:
 *      https://msdn.microsoft.com/en-us/library/windows/desktop/ms738563
 */
u_long address_conversion(QString address) {
    std::string str = address.toStdString();
    const char* cstr_addr = str.c_str();
    u_long dest = inet_addr(cstr_addr);
    // conversion failed, maybe a domain
    if (dest == INADDR_NONE) {
        dest = hostname_to_ip(address);
    }
    return dest;
}

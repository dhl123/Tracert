#include "traceroute.h"
#include "icmp_sender.h"
#include <QDebug>

traceroute::traceroute(QObject *parent, QString host, int packet_count, int max_hop) : QObject(parent) {
    this->host = host;
    this->packet_count = packet_count;
    this->max_hop = max_hop;
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        //TODO: ERROR HANDLER
        return ;
    }
    this->sender=new icmp_sender(wsa);
}

void traceroute::start_trace() {
    int ttl = 1;
    this->allowTrace = 1;
    while(ttl <= max_hop && allowTrace) {
        QList<int> delay;
        DECODE_RESULT result =sender->send_packet(host,ttl,0);
        QString ip_str = QString(inet_ntoa(result.ip_addr));
        delay.append(result.round_trip_time);
        if (result.error_code < 0) {
            if (result.error_code != SENDER_SEND_TIMEDOUT) {
                stop_trace();
                return;
            } else {
                emit on_trace_receive(ttl, ip_str, delay);
                qDebug() << ttl << " Request timed out";
                ttl += 1;
            }
        } else {
            if (result.type == ICMP_TIMEOUT) {
                emit on_trace_receive(ttl, ip_str, delay);
                ttl += 1;
                qDebug() << ttl << " " << ip_str << " " << result.round_trip_time;
            }
            if (result.type == ICMP_ECHO) {
                emit on_trace_receive(ttl, host, delay);
                qDebug() << ttl << " " << ip_str << " " << result.round_trip_time;
                stop_trace();
                qDebug() << "Trace completed.";
                return;
            }
        }
    }
}

void traceroute::stop_trace() {
    this->allowTrace = 0;
    emit on_trace_complete();
}

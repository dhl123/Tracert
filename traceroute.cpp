#include "traceroute.h"
#include "icmp_sender.h"
#include "utils.h"
#include <QDebug>
#include <QThreadPool>

traceroute::traceroute(QObject *parent, QString host, int packet_count, int max_hop) : QObject(parent) {
    this->host = host;
    this->packet_count = packet_count;
    this->max_hop = max_hop;
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        //TODO: ERROR HANDLER
        return;
    }
    this->sender = new icmp_sender(wsa);
}

void traceroute::start_trace() {
    int ttl = 1;
    this->allowTrace = 1;
    while (ttl <= max_hop && allowTrace) {
        //according to packet_count
        QList<int> delay;
        QString ip_str = "";
        for(int i=0; i<packet_count; i++) {
            DECODE_RESULT result = sender->send_packet(host,ttl,0);
            if (result.round_trip_time != -1) {
                ip_str = QString(inet_ntoa(result.ip_addr));
            }
            delay.append(result.round_trip_time);
            if (result.error_code < 0) {
                if (result.error_code != SENDER_SEND_TIMEDOUT) {
                    stop_trace();
                    return;
                } else {
                    if (i == packet_count - 1) {
                        emit on_trace_receive(ttl, "", delay);
                        ttl += 1;
                    }
                }
            } else {
                if (result.type == ICMP_TTL_EXCEEDED) {
                    if (i == packet_count - 1) {
                        emit on_trace_receive(ttl, ip_str, delay);
                        ttl += 1;
                    }
                }
                if (result.type == ICMP_ECHO) {
                    if (i == packet_count - 1) {
                        emit on_trace_receive(ttl, ip_str, delay);
                        stop_trace();
                        return;
                    }
                }
            }
        }
    }

    if (ttl >= max_hop) {
        stop_trace();
    }
}

void traceroute::stop_trace() {
    this->allowTrace = 0;
    emit on_trace_complete();
}

//slot
void traceroute::doWork() {
    QString result = "work";
    start_trace();
    result="work_done";
    emit resultReady(result);
}

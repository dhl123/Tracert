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
    QList<int> delay;
    while (ttl <= max_hop && allowTrace) {

        //according to packet_count
        for(int i=0;i<packet_count;i++){
//            QThreadPool::globalInstance()->start(sender);
            DECODE_RESULT result = sender->send_packet(host,ttl,0);
            QString ip_str = QString(inet_ntoa(result.ip_addr));
            delay.append(result.round_trip_time);
            if (result.error_code < 0) {
                if (result.error_code != SENDER_SEND_TIMEDOUT) {
                    stop_trace();
                    return;
                } else {
                    emit on_trace_receive(ttl, ip_str, delay);
                    qDebug() << ttl<<"packet"<<(i+1)<< " Request timed out" << QThread::currentThreadId()<<endl;
//                    ttl += 1;
                }
            } else {
                if (result.type == ICMP_TTL_EXCEEDED) {
                    emit on_trace_receive(ttl, ip_str, delay);
                    qDebug() << ttl <<"packet"<<(i+1)<< " " << ip_str << " " << result.round_trip_time;
                    qDebug() << ip_to_hostname(ip_str)<< QThread::currentThreadId()<<endl;
//                    ttl += 1;
                }
                if (result.type == ICMP_ECHO) {
                    emit on_trace_receive(ttl, host, delay);
                    qDebug() << ttl << "packet"<<(i+1)<<" " << ip_str << " " << result.round_trip_time;
                    qDebug() << ip_to_hostname(ip_str);
                    stop_trace();
                    qDebug() << "Trace completed.";
                    qDebug() << QThread::currentThreadId()<<endl;
                    return;
                }
            }
        }

        ttl+=1;
    }
}

void traceroute::stop_trace() {
    this->allowTrace = 0;
    emit on_trace_complete();
}

//slot
void traceroute::doWork(){
    QString result = "work";
    start_trace();
    result="work_done";
    emit resultReady(result);
}

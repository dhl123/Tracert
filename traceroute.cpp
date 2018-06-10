#include "traceroute.h"
#include<icmp_sender.h>
#include<icmp_header.h>
#include<qdebug.h>

traceroute::traceroute(QObject *parent, QString host, int packet_count):QObject(parent)
 {
    this->host=host;
    this->packet_count=packet_count;
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        //TODO: ERROR HANDLER
        return ;
    }
    this->sender=new icmp_sender(wsa);
}

void traceroute::start_trace() {
    int no;
    QString source_ip;
    QList<int> delay;
    int ttl=1;
    while(true){

        DECODE_RESULT result =sender->send_packet(host,ttl,0);



        if(result.error_code!=SENDER_SEND_TIMEDOUT){
            stop_trace();
            return ;
        }
        if(result.type==ICMP_TIMEOUT){
            ttl+=1;
            qDebug()<<ttl;
        }
        if(result.type==ICMP_ECHO_REPLY){
            delay.append(-1);
            emit on_trace_receive(no,source_ip, delay);
            break;
        }

        no=result.seq_no;
        source_ip=inet_ntoa(result.ip_addr);
        delay.append(result.round_trip_time);

        emit on_trace_receive(no,source_ip, delay);
    }


    emit on_trace_complete();

}

void traceroute::stop_trace() {

}

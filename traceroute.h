#ifndef TRACEROUTE_H
#define TRACEROUTE_H

#include <QObject>
#include <QList>
#include <Qthread>
#include<icmp_sender.h>

const int DEF_MAX_HOP = 30;

class traceroute : public QObject {
    Q_OBJECT
public:
    QString host;
    int packet_count;
    int max_hop;
    icmp_sender* sender;
    explicit traceroute(QObject *parent, QString host, int packet_count, int max_hop = DEF_MAX_HOP);
    void start_trace();
    void stop_trace();

private:
    int allowTrace;
//    QThread m_thread;

signals:
    void on_trace_receive(int no, QString source_ip, QList<int> delay);
    void on_trace_complete();
    void resultReady(const QString &result);
public slots:
    void doWork();

};

#endif // TRACEROUTE_H

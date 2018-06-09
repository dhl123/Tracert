#ifndef TRACEROUTE_H
#define TRACEROUTE_H

#include <QObject>
#include <QList>

class traceroute : public QObject {
    Q_OBJECT
public:
    explicit traceroute(QObject *parent, QString host, int packet_count);
    void start_trace();
    void stop_trace();

signals:
    void on_trace_receive(int no, QString source_ip, QList<int> delay);
    void on_trace_complete();

};

#endif // TRACEROUTE_H

#include "query_hostname_task.h"
#include <QString>
#include "utils.h"

query_hostname_task::query_hostname_task(QObject *parent) : QObject(parent) {

}

void query_hostname_task::run(int id, QString ip) {
    QString host = ip_to_hostname(ip);
    emit on_finish(id, host);
}

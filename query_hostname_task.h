#ifndef QUERY_HOSTNAME_TASK_H
#define QUERY_HOSTNAME_TASK_H

#include <QObject>
#include <QRunnable>


class query_hostname_task : public QObject {
    Q_OBJECT
public:
    explicit query_hostname_task(QObject *parent = nullptr);
    void run(int id, QString ip);
signals:
    void on_finish(int id, QString result);
public slots:
};

#endif // QUERY_HOSTNAME_TASK_H

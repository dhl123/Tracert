#ifndef QUERY_HOSTNAME_TASK_H
#define QUERY_HOSTNAME_TASK_H

#include <QObject>

class query_hostname_task : public QObject
{
    Q_OBJECT
public:
    explicit query_hostname_task(QObject *parent = nullptr);

signals:

public slots:
};

#endif // QUERY_HOSTNAME_TASK_H
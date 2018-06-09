#ifndef TRACEROUTE_H
#define TRACEROUTE_H

#include <QObject>

class traceroute : public QObject
{
    Q_OBJECT
public:
    explicit traceroute(QObject *parent = nullptr);

signals:

public slots:
};

#endif // TRACEROUTE_H
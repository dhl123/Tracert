#ifndef IP_LOCATOR_H
#define IP_LOCATOR_H

#include <QObject>

class ip_locator : public QObject
{
    Q_OBJECT
public:
    explicit ip_locator(QObject *parent = nullptr);

signals:

public slots:
};

#endif // IP_LOCATOR_H
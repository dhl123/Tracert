#ifndef IP_LOCATOR_H
#define IP_LOCATOR_H
#define _AFXDLL
#include <QObject>

const QString API_KEY = QString("XKPBZ-VM3EO-UI4W2-SCLNC-TJAKQ-AZFBH");

class ip_locator : public QObject {
    Q_OBJECT
public:
    explicit ip_locator(QObject *parent = nullptr);
    void send_request(QString ip, int id);

signals:
    void on_ip_located(int id, QString city, double longtitude, double latitude);
};

#endif // IP_LOCATOR_H

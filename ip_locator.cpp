#include "ip_locator.h"
#include <tchar.h>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>

ip_locator::ip_locator(QObject *parent) : QObject(parent) {

}

void ip_locator::send_request(QString ip, int id) {
    QString url = "http://freeapi.ipip.net/"+ip;
    QNetworkRequest request;
    request.setUrl(QUrl(url));

    QNetworkAccessManager* manage = new QNetworkAccessManager();
    manage->get(request);
    connect(manage, &QNetworkAccessManager::finished, this, [=](QNetworkReply *reply) {
        QString message = QString(reply->readAll());
        message.remove("\"");
        message.remove("[");
        message.remove("]");
        QString m1 = message.section(',', 1, 1);
        QString m2 = message.section(',', 2, 2);
        QString city;
        if(m1 == m2) {
            city = m1 + message.section(',', 3, 3);
        } else {
            city = m1 + m2 + message.section(',', 3, 3);
        }

        QString url = "http://apis.map.qq.com/ws/geocoder/v1/?address=" + city + "&&key=XKPBZ-VM3EO-UI4W2-SCLNC-TJAKQ-AZFBH";

        QNetworkRequest request;
        request.setUrl(QUrl(url));

        QNetworkAccessManager* manage = new QNetworkAccessManager();
        manage->get(request);
        connect(manage, &QNetworkAccessManager::finished, this, [=](QNetworkReply *reply) {

            QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll());
            QJsonObject json_message = jsonDocument.object();
            QJsonValue result = json_message.take("result");

            json_message = result.toObject();
            result = json_message.take("location");

            json_message = result.toObject();
            double lng=json_message.take("lng").toVariant().toDouble();
            double lat=json_message.take("lat").toVariant().toDouble();

            emit on_ip_located(id, city, lng, lat);
        });
    });
}



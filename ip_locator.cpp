#include "ip_locator.h"
#include <tchar.h>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <windows.h>

ip_locator::ip_locator(QObject *parent) : QObject(parent) {

}

void ip_locator::send_request(QString ip, int id) {
    QString url = "http://api.map.baidu.com/location/ip?ip=" + ip + "&ak=DF03bd597218e49903a0656fce0b1067&coor=gcj02";

    QNetworkRequest request;
    request.setUrl(QUrl(url));

    QNetworkAccessManager* manage = new QNetworkAccessManager();
    manage->get(request);
    connect(manage, &QNetworkAccessManager::finished, this, [=](QNetworkReply *reply) {
        QByteArray data = reply->readAll();
        QJsonDocument jsonDocument = QJsonDocument::fromJson(data);
        QJsonObject json_message = jsonDocument.object();
        QJsonValue result_base = json_message.take("content");

        json_message = result_base.toObject();
        QJsonValue result = json_message.take("point");

        json_message = result.toObject();
        double lng=json_message.take("y").toVariant().toDouble();
        double lat=json_message.take("x").toVariant().toDouble();


        json_message = result_base.toObject();
        QString city = json_message.take("address").toString();
        emit on_ip_located(id, city, lng, lat);
    });
}



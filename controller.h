#ifndef CONTROLLER_H
#define CONTROLLER_H
#include <traceroute.h>
#include <QDebug>
class Controller : public QObject
{
    Q_OBJECT
    QThread workerThread;
public:
    Controller(QString string,int k) {
        traceroute *worker = new traceroute(nullptr,string,k);
        worker->moveToThread(&workerThread);
        connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
        connect(this, &Controller::operate_, worker, &traceroute::doWork);
        connect(worker, &traceroute::resultReady, this, &Controller::handleResults);
        workerThread.start();
    }
    ~Controller() {
        workerThread.quit();
        workerThread.wait();
    }
public slots:
    void handleResults(const QString &t){
       qDebug()<<t;
    }
signals:
    void operate_();
};
#endif // CONTROLLER_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include "ip_locator.h"
#include "traceroute.h"
#include "query_hostname_task.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    void initModel();
    void appendRow(int ttl, QString ip, QList<int> delay);
    ip_locator *locator;
    query_hostname_task *query;
    QStandardItemModel *trace_model;
    traceroute *tracer;
    void on_trace_receive(int ttl, QString source_ip, QList<int> delay);
    void on_ip_located(int id, QString city, double longtitude, double latitude);
    void on_ip_resolved(int id, QString host);
    void updateRow(int row, int col, QString value);
    void on_trace_complete();
};

#endif // MAINWINDOW_H

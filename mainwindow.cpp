#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHeaderView>
#include <QStandardItemModel>
#include <QStringList>
#include <QStringBuilder>
#include "ip_locator.h"
#include "traceroute.h"
#include "query_hostname_task.h"
#include <QDebug>
#include <QThreadPool>
#include <QFuture>
#include <QtConcurrent>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setShowGrid(false);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    initModel();
    locator = new ip_locator(this);
    query = new query_hostname_task(this);
    connect(locator, &ip_locator::on_ip_located, this, &MainWindow::on_ip_located);
    connect(query, &query_hostname_task::on_finish, this, &MainWindow::on_ip_resolved);
}

void MainWindow::initModel() {
    QStandardItemModel *model = new QStandardItemModel(this);
    this->trace_model = model;
    ui->tableView->setModel(model);

    QStringList labels;
    labels.append("Delay");
    labels.append("IP");
    labels.append("Hostname");
    labels.append("Location");
    model->setHorizontalHeaderLabels(labels);
}

void MainWindow::appendRow(int ttl, QString ip, QList<int> delay) {
    QStringList list;
    for(int i=0; i<delay.count(); i++) {
        int value = delay.at(i);
        QString str;
        if (value == -1) {
            str = "*";
        } else if (value == 0) {
            str = "<1ms";
        } else {
            str = QString::number(value) + "ms";
        }
        list.append(str);
    }
    QString delays = list.join(' ');
    QString display_ip = ip;

    if (ip.length() == 0) {
        display_ip = "Request timed out.";
    }
    QStandardItem *item1 = new QStandardItem();
    item1->setData(delays, 0);
    QStandardItem *item2 = new QStandardItem();
    item2->setData(display_ip, 0);
    QStandardItem *item3 = new QStandardItem();
    item3->setData("Resolving...", 0);
    QStandardItem *item4 = new QStandardItem();
    item4->setData("Resolving...", 0);

    if (ip.length() == 0) {
        item3->setData("", 0);
        item4->setData("", 0);
    }

    QList<QStandardItem*> items;
    items.append(item1);
    items.append(item2);
    items.append(item3);
    items.append(item4);
    this->trace_model->appendRow(items);


    if (ip.length() != 0) {
        locator->send_request(ip, ttl);
        QtConcurrent::run(query, &query_hostname_task::run, ttl, ip);
    }
}

void MainWindow::on_trace_receive(int ttl, QString source_ip, QList<int> delay) {
    this->appendRow(ttl, source_ip, delay);
}

void MainWindow::on_trace_complete() {
    QMessageBox::information(this, "iTracert", "Trace complete.");
}

void MainWindow::on_ip_located(int id, QString city, double longtitude, double latitude) {
    if (longtitude < 0 || latitude < 0) {
        this->updateRow(id - 1, 3, city);
    } else {
        this->updateRow(id - 1, 3, city + " (" + QString::number(longtitude) + "E, " + QString::number(latitude) + "N)");
    }
}

void MainWindow::on_ip_resolved(int id, QString host) {
    this->updateRow(id - 1, 2, host);
}

void MainWindow::updateRow(int row, int col, QString value) {
    this->trace_model->item(row, col)->setData(value, 0);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_pushButton_clicked() {
    this->trace_model->clear();
    this->initModel();
    this->tracer = new traceroute(this, ui->lineEdit->text(), ui->lineEdit_2->text().toInt());
    ui->label_2->setText("Tracing route to " + ui->lineEdit->text() + " over a maximum of "+ ui->lineEdit_2->text() + " hops:");
    connect(this->tracer, &traceroute::on_trace_receive, this, &MainWindow::on_trace_receive);
    connect(this->tracer, &traceroute::on_trace_complete, this, &MainWindow::on_trace_complete);
    QtConcurrent::run(tracer, &traceroute::start_trace);
}

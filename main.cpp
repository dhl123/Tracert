#include <stdio.h>
#include <stdlib.h>
#define HAVE_REMOTE
#include <WinSock2.h>
#include <iphlpapi.h>
#include <icmpapi.h>
#include "icmp_sender.h"
#include "ip_locator.h"
#include "traceroute.h"
#include "utils.h"
#include <iostream>
#include <QCoreApplication>
#include <QApplication>
#include <QDebug>
#include "icmp_sender.h"
#include "traceroute.h"
#include "mainwindow.h"
#define TARGET "220.181.57.216"

int main(int argc,char** argv) {
    typedef QList<int> IntList;
    qRegisterMetaType< IntList >( "IntList" );
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}

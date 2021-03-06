QT += network
QT += core
QT += gui widgets
QT += concurrent

CONFIG += c++11

TARGET = itracert

TEMPLATE = app

SOURCES += main.cpp \
    icmp_sender.cpp \
    ip_locator.cpp \
    traceroute.cpp \
    utils.cpp \
    send_thread.cpp \
    mainwindow.cpp \
    query_hostname_task.cpp

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS


LIBS += -liphlpapi -luser32 -lws2_32 # for using Windows API

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    icmp_sender.h \
    ip_locator.h \
    traceroute.h \
    utils.h \
    send_thread.h \
    mainwindow.h \
    query_hostname_task.h

FORMS += \
    mainwindow.ui

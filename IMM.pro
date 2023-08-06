#-------------------------------------------------
#
# Project created by QtCreator 2023-08-01T14:00:45
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FeiQ
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    mediator/INetMediator.cpp \
    mediator/UdpMediator.cpp \
    net/UdpNet.cpp \
    mediator/tcpClientMediator.cpp \
    ckernel.cpp \
    net/tcpClient.cpp \
    cfriendlist.cpp \
    cfrienditem.cpp \
    chatdialog.cpp

HEADERS  += mainwindow.h \
    pakdef.h \
    mediator/INetMediator.h \
    mediator/UdpMediator.h \
    net/INet.h \
    net/UdpNet.h \
    ckernel.h \
    mediator/tcpClientMediator.h \
    net/tcpClient.h \
    cfriendlist.h \
    cfrienditem.h \
    chatdialog.h

INCLUDEPATH += ./net  \
                ./mediator

LIBS += -lws2_32

FORMS    += mainwindow.ui \
    cfriendlist.ui \
    cfrienditem.ui \
    chatdialog.ui

RESOURCES += \
    source.qrc

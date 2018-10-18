#-------------------------------------------------
#
# Project created by QtCreator 2017-01-30T21:36:37
#
#-------------------------------------------------

QT       += core gui network serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = "controller-pc"
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

DEFINES += SDN_CONTROLLER_PC

DEFINES += DEBUG_SDN

DEFINES += SDNADDR_SIZE=2
DEFINES += DIGRAPH_VERTICE_ID_SIZE=SDNADDR_SIZE

DEFINES += SDN_MAX_PACKET_SIZE=115

DEFINES += SDN_SEND_QUEUE_SIZE=100
DEFINES += SDN_RECV_QUEUE_SIZE=100

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#INCLUDEPATH+=../../

include(controller-pc.pro.contiki)

INCLUDEPATH+=$$CONTIKI/core/
INCLUDEPATH+=../../sdn-common/
INCLUDEPATH+=../
INCLUDEPATH+=../digraph/

SOURCES += main.cpp\
        mainwindow.cpp \
    serializer.cpp \
    serialconnector.cpp \
    ../../sdn-common/control-flow-table.c \
    ../../sdn-common/data-flow-table.c \
    ../../sdn-common/sdn-addr.c \
    ../../sdn-common/sdn-debug.c \
    ../../sdn-common/sdn-packetbuf.c \
    ../../sdn-common/sdn-queue.c \
    ../../sdn-common/sdn-send-packet.c \
    ../../sdn-common/sdn-process-packets.c \
    ../../sdn-common/sdn-unknown-route.c \
    $$CONTIKI/core/lib/list.c \
    ../digraph/digraph.c \
    ../digraph/dijkstra.c \
    ../controller-core.c \
    ../flow-table-cache.c \
    ../sdn-serial-send.c \
    ../sdn-process-packets-controller.c \
    mainwindow_wrapper.cpp

HEADERS  += mainwindow.h \
    serializer.h \
    serialconnector.h \
    ../../sdn-common/control-flow-table.h \
    ../../sdn-common/data-flow-table.h \
    ../../sdn-common/sdn-addr.h \
    ../../sdn-common/sdn-constants.h \
    ../../sdn-common/sdn-debug.h \
    ../../sdn-common/sdn-packetbuf.h \
    ../../sdn-common/sdn-protocol.h \
    ../../sdn-common/sdn-queue.h \
    ../../sdn-common/sdn-send-packet.h \
    ../../sdn-common/sdn-process-packets.h \
    ../../sdn-common/sdn-unknown-route.h \
    $$CONTIKI/core/lib/list.h \
    ../digraph/digraph.h \
    ../digraph/dijkstra.h \
    ../controller-core.h \
    ../flow-table-cache.h \
    ../sdn-serial-send.h \
    ../sdn-process-packets-controller.h \
    mainwindow_wrapper.h

FORMS    += mainwindow.ui

DISTFILES += \
    controller-pc.pro.contiki



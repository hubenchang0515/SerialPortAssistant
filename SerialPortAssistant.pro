#-------------------------------------------------
#
# Project created by QtCreator 2016-12-22T18:18:24
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SerialPortAssistant
TEMPLATE = app


SOURCES += main.cpp\
        serialportassistant.cpp \
    helpdialog.cpp

HEADERS  += serialportassistant.h \
    helpdialog.h

FORMS    += serialportassistant.ui

CONFIG += c++11

RESOURCES += \
    rsc.qrc

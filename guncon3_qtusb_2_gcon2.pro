#-------------------------------------------------
#
# Project created by QtCreator 2019-01-27T13:36:07
#
#-------------------------------------------------

QT       += core gui
QT += multimedia serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += link_pkgconfig
PKGCONFIG += libevdev
LIBS += -levdev

TARGET = guncon3_qtusb_2_gcon2
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include (QtUsb/src/QtUsb.pri)
SOURCES += main.cpp\
        mainwindow.cpp \
    guncon_usb.cpp \
    calibration.cpp \
    events.cpp

HEADERS  += mainwindow.h \
    guncon_usb.h \
    calibration.h \
    events.h

FORMS    += mainwindow.ui

RESOURCES += \
    img_audio.qrc

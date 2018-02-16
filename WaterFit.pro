#-------------------------------------------------
#
# Project created by QtCreator 2015-03-25T15:58:12
#
#-------------------------------------------------

QT       += core gui printsupport xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WaterFit
TEMPLATE = app

INCLUDEPATH += /usr/local/qwt-6.1.3/include/
LIBS += -L/usr/local/qwt-6.1.3/lib/ -lqwt

SOURCES += main.cpp\
        waterfit.cpp \
    WaterFitInput.cpp \
    myplot2d.cpp

HEADERS  += waterfit.h \
    WaterFitInput.h \
    myplot2d.h \
    pixmaps.h

FORMS    += waterfit.ui

DEFINES  += XVMC

CONFIG   += qwt

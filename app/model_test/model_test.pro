#-------------------------------------------------
#
# Project created by QtCreator 2013-12-03T12:55:41
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = model_test
TEMPLATE = app


SOURCES += main.cc\
        widget.cc \
    ../../src/network_model.cc \
    ../../src/network_manager.cc \
    ../../src/network_analysis.cc

HEADERS  += widget.h \
    ../../src/network_model.h \
    ../../src/network_manager.h \
    ../../src/network_analysis.h

FORMS    += widget.ui

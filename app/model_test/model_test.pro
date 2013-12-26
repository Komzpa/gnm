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
    ../../gnm/gnmmanager.cpp \
    ../../gnm/ogrgnmdatasource.cpp \
    ../../gnm/ogrgnmdriver.cpp \
    ../../gnm/ogrgnmlayer.cpp

HEADERS  += widget.h \
    ../../gnm/ogr_gnm.h \
    ../../gnm/gnmmanager.h \
    ../../gnm/gnmanalysis.h \
    ../../gnm/gnmcore.h

LIBS += c:\OSGeo4W\lib\gdal_i.lib

INCLUDEPATH += c:\OSGeo4W\include

FORMS    += widget.ui

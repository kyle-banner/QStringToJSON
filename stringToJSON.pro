#-------------------------------------------------
#
# Project created by QtCreator 2014-07-24T21:32:36
#
#-------------------------------------------------

QT       += core gui webkit network webkitwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = stringToJSON
TEMPLATE = app

CONFIG += openssl
INCLUDEPATH += &&system{PWD}/openssl-bin

SOURCES += main.cpp\
        stringtojson.cpp \
    json.cpp

HEADERS  += stringtojson.h \
    json.h

FORMS    += stringtojson.ui

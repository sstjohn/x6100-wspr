TEMPLATE = lib
TARGET = qinj

QT = core gui widgets network

SOURCES += qinj.cpp XWsprWidget.cpp
HEADERS += qinj.h XWsprWidget.h
LIBS += -lpython3
CONFIG += link_pkgconfig no_keywords
PKGCONFIG += python3

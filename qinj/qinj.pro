TEMPLATE = lib
TARGET = qinj

QT = core gui widgets

SOURCES += qinj.cpp
HEADERS += qinj.h
LIBS += -lpython3.8
CONFIG += link_pkgconfig no_keywords
PKGCONFIG += python3

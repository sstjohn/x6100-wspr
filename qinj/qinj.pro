TEMPLATE = lib
TARGET = qinj

QT = core gui widgets network

SOURCES += qinj.cpp app_threads.cpp XWsprWidget.cpp AppButtonWatcher.cpp Injection.cpp InjectedMessageBox.cpp
HEADERS += qinj.h app_threads.h XWsprWidget.h AppButtonWatcher.h Injection.h InjectedMessageBox.h
LIBS += -lpython3
CONFIG += link_pkgconfig no_keywords
PKGCONFIG += python3

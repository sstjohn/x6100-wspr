TEMPLATE = lib
TARGET = qinj

QT = core gui widgets network dbus

SOURCES += qinj.cpp app_threads.cpp XWsprWidget.cpp InjectedEventFilter.cpp Injection.cpp InjectedMessageBox.cpp
HEADERS += qinj.h app_threads.h XWsprWidget.h InjectedEventFilter.h Injection.h InjectedMessageBox.h
LIBS += -lpython3
CONFIG += link_pkgconfig no_keywords
PKGCONFIG += python3

dbus_conf.path = /etc/dbus-1/system.d
dbus_conf.files = xwspr-dbus.conf
INSTALLS += dbus_conf

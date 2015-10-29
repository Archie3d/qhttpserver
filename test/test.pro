QT       += core network
QT       -= gui

TARGET = testhttp
CONFIG   += console
CONFIG   -= app_bundle

HEADERS +=

SOURCES += \
           main.cpp

INCLUDEPATH += ../httpserver

CONFIG(debug, debug|release) {
    LIBS += -L$$OUT_PWD/../httpserver/debug
} else {
    LIBS += -L$$OUT_PWD/../httpserver/release
}

win32:LIBS += httpserver.lib
unix:LIBS += httpserver.a


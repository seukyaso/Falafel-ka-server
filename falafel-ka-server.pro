TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    src/cfalafelserver.cpp

HEADERS = \
    src/cfalafelserver.h

LIBS += -pthread

# -------------------------------------------------
# Project created by QtCreator 2009-04-04T20:01:13
# -------------------------------------------------
QT += xml
QT -= gui
TARGET = OSMParser
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \
    osmparser.cpp \
    Way.cpp \
    Transform.cpp \
    App.cpp
HEADERS += osmparser.h \
    Way.h \
    Transform.h \
    App.h

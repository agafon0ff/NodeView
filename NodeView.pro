#-------------------------------------------------
#
# Project created by QtCreator 2017-05-29T14:08:48
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NodeView
TEMPLATE = app


SOURCES += main.cpp \
    nodeview.cpp \
    nodeitem.cpp \
    ropeitem.cpp \
    portitem.cpp

HEADERS  += \
    nodeview.h \
    nodeitem.h \
    ropeitem.h \
    portitem.h

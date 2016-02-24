#-------------------------------------------------
#
# Project created by QtCreator 2016-01-31T16:43:17
#
#-------------------------------------------------

QT       += core gui
CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = logic-calc
TEMPLATE = app


SOURCES += main.cpp\
        logiccalcwindow.cpp \
    lexer.cpp \
    parser.cpp

HEADERS  += logiccalcwindow.h \
    enums.h \
    lexer.hpp \
    parser.hpp \
    unistd.h

FORMS    += logiccalcwindow.ui

TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        TwoDimManipulator.cpp \
        main.cpp

HEADERS += \
    TwoDimManipulator
include(../osg.pri)

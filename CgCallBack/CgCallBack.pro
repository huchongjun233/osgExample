TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        CgCallbacks.cpp \
        main.cpp
include(../osg.pri)

HEADERS += \
    CgCallbacks

TEMPLATE = app
CONFIG += console c++11 opengl
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        CloudBlock.cpp \
        main.cpp
include(../osg.pri)

HEADERS += \
    CloudBlock

TEMPLATE = app
CONFIG += console c++11 opengl
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        NurbsSurface.cpp \
        main.cpp

HEADERS += \
    NurbsSurface
include(../osg.pri)

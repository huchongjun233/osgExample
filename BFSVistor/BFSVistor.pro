TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        BFSVisitor.cpp \
        main.cpp

HEADERS += \
    BFSVisitor
include(../osg.pri)

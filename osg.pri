
HEADERS += $$PWD/common/CommonFunctions
SOURCES += $$PWD/common/CommonFunctions.cpp
win32:CONFIG(debug, debug|release):{
 LIBS += -LE:/environment/osg/osg365/lib/
 LIBS += -lOpenThreadsd\
         -losgd\
         -losgAnimationd\
         -losgDBd\
         -losgFXd\
         -losgGAd\
         -losgManipulatord\
         -losgParticled\
         -losgPresentationd\
         -losgShadowd\
         -losgSimd\
         -losgTerraind\
         -losgTextd\
         -losgUId\
         -losgUtild\
         -losgUId\
         -losgViewerd\
         -losgVolumed\
         -losgWidgetd\
}else:win32:CONFIG(release, debug|release):{
 LIBS += -LE:/environment/osg/osg365/lib/
 LIBS += -lOpenThreads\
         -losg\
         -losgAnimation\
         -losgDB\
         -losgFX\
         -losgGA\
         -losgManipulator\
         -losgParticle\
         -losgPresentation\
         -losgShadow\
         -losgSim\
         -losgTerrain\
         -losgText\
         -losgUI\
         -losgUtil\
         -losgUI\
         -losgViewer\
         -losgVolume\
         -losgWidget\
}

win32-msvc* {
    QMAKE_CXXFLAGS *=  /wd"4100"
    contains (QMAKE_CXXFLAGS_WARN_ON, -w34100) : QMAKE_CXXFLAGS_WARN_ON -= -w34100
}

INCLUDEPATH += E:/environment/osg/osg365/include
INCLUDEPATH += $$PWD/common

DEPENDPATH += E:/environment/osg/osg365/include


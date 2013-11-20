#-------------------------------------------------
#
# Project created by QtCreator 2013-11-16T15:00:46
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ZyberPlayer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    videocontainer.cpp \
    videoinfo.cpp

HEADERS  += mainwindow.h \
    videocontainer.h \
    videoinfo.h

FORMS    += mainwindow.ui

mac: LIBS += -F/Library/Frameworks/ -framework GStreamer
INCLUDEPATH += /Library/Frameworks/GStreamer.framework/Headers
DEPENDPATH += /Library/Frameworks/GStreamer.framework/Headers

OTHER_FILES += \
    Entypo.ttf

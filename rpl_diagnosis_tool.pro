#-------------------------------------------------
#
# Project created by QtCreator 2013-06-26T14:10:10
#
#-------------------------------------------------

QT       += core gui
CONFIG   += debug_and_release debug_and_release_target

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = rpl_diagnosis_tool
TEMPLATE = app

LIBS += $$PWD/../analyzer/dist/Debug/GNU-Linux-x86/libanalyzer.so

INCLUDEPATH += $$PWD/../analyzer/



SOURCES += main.cpp\
        MainWindow.cpp \
    rpl/rplTreeScene.cpp \
    rpl/rplNode.cpp \
    rpl/rplNetworkInfoManager.cpp \
    rpl/rplLink.cpp

HEADERS  += MainWindow.h \
    rpl/rplTreeScene.h \
    rpl/rplNode.h \
    rpl/rplNetworkInfoManager.h \
    rpl/rplLink.h

FORMS    += MainWindow.ui

RESOURCES += \
    Resource.qrc

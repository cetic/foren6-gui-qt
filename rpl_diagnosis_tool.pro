#-------------------------------------------------
#
# Project created by QtCreator 2013-06-26T14:10:10
#
# Use this command to generate Makefiles in the current directory:
#  qmake-qt4 /home/am/git/lowpan-diagnosis-tool/gui-qt/rpl_diagnosis_tool.pro -r -spec linux-g++ CONFIG+=debug
#
# Then use 'make debug' or 'make release' to compile the project.
# The default target is debug (unless you didn't specified CONFIG+=debug to qmake when generating Makefiles)
#
# The analyzer library (libanalyser.so) must be compiled before this project.
#
#-------------------------------------------------

QT       += core gui
CONFIG   += debug_and_release debug_and_release_target

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = rpl_diagnosis_tool
TEMPLATE = app

LIBS += -L$$PWD/../analyzer/dist/Debug/GNU-Linux-x86/
LIBS += -lanalyzer

INCLUDEPATH += $$PWD/../analyzer/



SOURCES += main.cpp\
        MainWindow.cpp \
    rpl/rplTreeScene.cpp \
    rpl/rplNode.cpp \
    rpl/rplNetworkInfoManager.cpp \
    rpl/rplLink.cpp \
    EventLog.cpp \
    InformationWidget.cpp \
    SnifferDialog.cpp \
    OpenedSniffersModel.cpp \
    VersionSlider.cpp

HEADERS  += MainWindow.h \
    rpl/rplTreeScene.h \
    rpl/rplNode.h \
    rpl/rplNetworkInfoManager.h \
    rpl/rplLink.h \
    EventLog.h \
    InformationWidget.h \
    SnifferDialog.h \
    OpenedSniffersModel.h \
    VersionSlider.h

FORMS    += MainWindow.ui \
    InformationWidget.ui \
    SnifferDialog.ui \
    VersionSlider.ui

RESOURCES += \
    Resource.qrc

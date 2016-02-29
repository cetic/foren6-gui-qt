#
# This file is part of Foren6, a 6LoWPAN Diagnosis Tool
# Copyright (C) 2013, CETIC
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#

#
# \file
#         Foren6 Qt Project Configuration File
# \author
#         Foren6 Team <foren6@cetic.be>
#         http://cetic.github.io/foren6/credits.html
#

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

TARGET = foren6
TEMPLATE = app

LIBS += -L$$PWD/../analyzer/dist/Debug/GNU-Linux-x86/
LIBS += -lrplanalyzer

INCLUDEPATH += $$PWD/../analyzer/src/

APP_CAPTURE_BUNDLE.files = ../capture/bin/libinterface_pcap.dylib ../capture/bin/libinterface_snif.dylib ../capture/bin/libinterface_sensnif.dylib  ../capture/bin/libinterface_cc2531.dylib
APP_CAPTURE_BUNDLE.path = Contents/MacOS

QMAKE_BUNDLE_DATA += APP_CAPTURE_BUNDLE



SOURCES += main.cpp\
        MainWindow.cpp \
    rpl/rplTreeScene.cpp \
    rpl/rplNode.cpp \
    rpl/rplNetworkInfoManager.cpp \
    rpl/rplLink.cpp \
    SimpleTreeItem.cpp \
    SimpleTreeModel.cpp \
    EventLog.cpp \
    InformationWidget.cpp \
    IOverlayModel.cpp \
    PacketWidget.cpp \
    SnifferDialog.cpp \
    OpenedSniffersModel.cpp \
    VersionSlider.cpp \
    overlays/NormalOverlay.cpp \
    overlays/DodagOverlay.cpp \
    GraphView.cpp \
    overlays/RplInstanceOverlay.cpp \
    overlays/ParentChildrenOverlay.cpp \
    overlays/RankDepthOverlay.cpp \
    overlays/TrafficOverlay.cpp \
    overlays/MaxDaoIntervalOverlay.cpp \
    overlays/MaxDioIntervalOverlay.cpp \
    Timeline.cpp \
    overlays/RankErrorOverlay.cpp \
    overlays/RouteLoopErrorOverlay.cpp \
    overlays/ErrorsOverlay.cpp \
    overlays/IPMismatchErrorOverlay.cpp \
    overlays/DodagMismatchErrorOverlay.cpp \
    SettingsDialog.cpp

HEADERS  += MainWindow.h \
    rpl/rplTreeScene.h \
    rpl/rplNode.h \
    rpl/rplNetworkInfoManager.h \
    rpl/rplLink.h \
    SimpleTreeItem.h \
    SimpleTreeModel.h \
    EventLog.h \
    InformationWidget.h \
    PacketWidget.h \
    SnifferDialog.h \
    OpenedSniffersModel.h \
    VersionSlider.h \
    IOverlayModel.h \
    overlays/NormalOverlay.h \
    overlays/DodagOverlay.h \
    GraphView.h \
    overlays/RplInstanceOverlay.h \
    overlays/ParentChildrenOverlay.h \
    overlays/RankDepthOverlay.h \
    overlays/TrafficOverlay.h \
    overlays/MaxDaoIntervalOverlay.h \
    overlays/MaxDioIntervalOverlay.h \
    Timeline.h \
    overlays/RankErrorOverlay.h \
    overlays/RouteLoopErrorOverlay.h \
    overlays/ErrorsOverlay.h \
    overlays/IPMismatchErrorOverlay.h \
    overlays/DodagMismatchErrorOverlay.h \
    SettingsDialog.h

FORMS    += MainWindow.ui \
    InformationWidget.ui \
    PacketWidget.ui \
    SnifferDialog.ui \
    VersionSlider.ui \
    GraphView.ui \
    About.ui \
    SettingsDialog.ui

RESOURCES += \
    Resource.qrc

#Icons
#MacOSX
ICON = resources/logo/foren6-400p-alpha.icns

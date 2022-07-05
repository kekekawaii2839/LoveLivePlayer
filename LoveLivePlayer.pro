#-------------------------------------------------
#
# Project created by QtCreator 2021-07-18T22:53:18
#
#-------------------------------------------------

QT       += core gui multimedia winextras

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LoveLivePlayer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    lrc.cpp \
    CustomClasses\qsliderpro.cpp \
    CustomClasses\qlistpushbutton.cpp \
    CustomClasses\qtitlewidget.cpp \
    config.cpp \
    CustomClasses\scrolllabel.cpp \
    desktoplyricwindow.cpp \
    quickselect.cpp \
    CustomClasses\qclickwidget.cpp \
    CustomClasses\laudioplayer.cpp \
    CustomClasses\lvideowidget.cpp \
    CustomClasses\animatedscrollbar.cpp \
    songinfo.cpp \
    CustomClasses\qmaskwidget.cpp \
    CustomClasses\qsmarttextwidget.cpp

HEADERS += \
        mainwindow.h \
    lrc.h \
    CustomClasses\qsliderpro.h \
    CustomClasses\qlistpushbutton.h \
    CustomClasses\qtitlewidget.h \
    config.h \
    CustomClasses\scrolllabel.h \
    desktoplyricwindow.h \
    globalvariable.h \
    quickselect.h \
    CustomClasses\qclickwidget.h \
    CustomClasses\laudioplayer.h \
    CustomClasses\lvideowidget.h \
    CustomClasses\animatedscrollbar.h \
    songinfo.h \
    CustomClasses\qmaskwidget.h \
    CustomClasses\qsmarttextwidget.h

FORMS += \
        mainwindow.ui \
    desktoplyricwindow.ui \
    quickselect.ui \
    CustomClasses\lvideowidget.ui \
    CustomClasses\qmaskwidget.ui

RESOURCES += \
    pic.qrc \
    font.qrc

win32: LIBS += -L$$PWD/../../Qt/libzplay-2.02-sdk/libzplay-2.02-sdk/C++/ -lzplay

INCLUDEPATH += $$PWD/../../Qt/libzplay-2.02-sdk/libzplay-2.02-sdk/C++
DEPENDPATH += $$PWD/../../Qt/libzplay-2.02-sdk/libzplay-2.02-sdk/C++

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../../Qt/libzplay-2.02-sdk/libzplay-2.02-sdk/C++/libzplay.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/../../Qt/libzplay-2.02-sdk/libzplay-2.02-sdk/C++/libzplay.a

LIBS += C:/Windows/System32/GdiPlus.dll
LIBS += -lgdi32
LIBS += -luser32

INCLUDEPATH += $$PWD/mpv
include ($$PWD/mpv/mpv.pri)

CONFIG += resources_big #解决qrc文件中资源文件过大导致无法编译的错误

VERSION = 1.23.1.9#版本信息 major.minor.patch.build

QMAKE_TARGET_COMPANY = "HopUp Tech"#公司名称

QMAKE_TARGET_PRODUCT = "LoveLivePlayer"#产品名称

QMAKE_TARGET_DESCRIPTION = "by 33ba6_2839 in HopUp Tech"#文件说明

QMAKE_TARGET_COPYRIGHT = ""#版权信息

RC_ICONS = rec/icon.ico

RC_LANG = 0x0004#语言:中文(简体)

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
    qsliderpro.cpp \
    qlistpushbutton.cpp \
    qtitlewidget.cpp \
    config.cpp \
    scrolllabel.cpp \
    desktoplyricwindow.cpp \
    quickselect.cpp \
    qclickwidget.cpp

HEADERS += \
        mainwindow.h \
    lrc.h \
    qsliderpro.h \
    qlistpushbutton.h \
    qtitlewidget.h \
    config.h \
    scrolllabel.h \
    desktoplyricwindow.h \
    globalvariable.h \
    quickselect.h \
    qclickwidget.h

FORMS += \
        mainwindow.ui \
    desktoplyricwindow.ui \
    quickselect.ui

RESOURCES += \
    pic.qrc \
    font.qrc

win32: LIBS += -L$$PWD/../../Qt/libzplay-2.02-sdk/libzplay-2.02-sdk/C++/ -lzplay

INCLUDEPATH += $$PWD/../../Qt/libzplay-2.02-sdk/libzplay-2.02-sdk/C++
DEPENDPATH += $$PWD/../../Qt/libzplay-2.02-sdk/libzplay-2.02-sdk/C++

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../../Qt/libzplay-2.02-sdk/libzplay-2.02-sdk/C++/zplay.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/../../Qt/libzplay-2.02-sdk/libzplay-2.02-sdk/C++/libzplay.a

LIBS += C:/Windows/System32/GdiPlus.dll
LIBS += -lgdi32
LIBS += -luser32

ICON = icon.ico

DISTFILES += \
    logo.rc

RC_FILE += logo.rc

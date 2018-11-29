#-------------------------------------------------
#
# Project created by QtCreator 2018-11-02T09:27:30
#
#-------------------------------------------------

QT       += core gui axcontainer xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TM-BM200_monitor
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
        tkbmwidget.cpp \
    logindialog.cpp \
    monitordialog.cpp \
    ctlcan.cpp

HEADERS += \
        tkbmwidget.h \
    logindialog.h \
    monitordialog.h \
    config.h \
    data_organize.h \
    static_string.h

FORMS += \
        tkbmwidget.ui \
    logindialog.ui \
    monitordialog.ui

win32 {
QMAKE_LFLAGS_WINDOWS = /SUBSYSTEM:WINDOWS,5.01
QMAKE_LFLAGS_CONSOLE = /SUBSYSTEM:CONSOLE,5.01

DEFINES += _ATL_XP_TARGETING
QMAKE_CFLAGS += /D_USING_V140_SDK71_
QMAKE_CXXFLAGS += /D_USING_V140_SDK71_
LIBS += -L$$quote(C:\Program Files (x86)\Microsoft SDKs\Windows\v7.1A\Lib)
INCLUDEPATH += $$quote(C:\Program Files (x86)\Microsoft SDKs\Windows\v7.1A\Include)
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

CONFIG += C++11
QMAKE_CFLAGS += -std=c99

win32: LIBS += -L$$PWD/build/ -lControlCAN

INCLUDEPATH += $$PWD/build
DEPENDPATH += $$PWD/build

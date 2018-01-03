QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TaskTimer
TEMPLATE = app
#DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        main.cpp \
        mainwindow.cpp

HEADERS += \
        mainwindow.h \
    dialog.h

FORMS += \
        mainwindow.ui

RESOURCES += \
    tasktimer.qrc

DISTFILES += \
    TaskTimer.rc \
    README.md

win32:RC_FILE = TaskTimer.rc

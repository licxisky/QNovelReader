#-------------------------------------------------
#
# Project created by QtCreator 2017-03-01T23:06:28
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QNovelReader
TEMPLATE = app


SOURCES += main.cpp\
        qnovelreader.cpp \
    qchapter.cpp \
    qnovel.cpp \
    qreadhistory.cpp \
    qrule.cpp \
    qsearchhistory.cpp \
    qthreadgetchapter.cpp \
    qthreadgetchapters.cpp \
    qthreadgetnovels.cpp \
    qthreaddownloadnovelchapter.cpp

HEADERS  += qnovelreader.h \
    qchapter.h \
    qnovel.h \
    qreadhistory.h \
    qrule.h \
    qsearchhistory.h \
    qthreadgetchapter.h \
    qthreadgetchapters.h \
    qthreadgetnovels.h \
    qthreaddownloadnovelchapter.h


FORMS    += \
    qnovelreader.ui

QT += network

RESOURCES += \
    source.qrc

#-------------------------------------------------
#
# Project created by QtCreator 2020-03-10T20:55:33
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = FLySMPS
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

CONFIG += c++11

SOURCES += \
    logfilewriter.cpp \
    loggercategories.cpp \
    capout.cpp \
    controlout.cpp \
    diodeout.cpp \
    fbptransformer.cpp \
    main.cpp \
    FLySMPS.cpp \
    diodebridge.cpp \
    bulkcap.cpp \
    outfilter.cpp \
    powsuppsolve.cpp \
    qcustomplot.cpp \
    swmosfet.cpp

HEADERS += \
    FLySMPS.h \
    logfilewriter.h \
    loggercategories.h \
    capout.h \
    controlout.h \
    diodebridge.h \
    bulkcap.h \
    diodeout.h \
    fbptransformer.h \
    outfilter.h \
    powsuppsolve.h \
    qcustomplot.h \
    swmosfet.h

FORMS += \
        FLySMPS.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res/flsmps_res.qrc

QT       += core gui
QT += serialbus
QT += serialport
QT += axcontainer

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++11
TARGET = "ModBus Master"

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    formchart.cpp \
    formgroup.cpp \
    formouttable.cpp \
    main.cpp \
    mainwindow.cpp \
    modbusdialog.cpp \
    outdialog.cpp \
    qcustomplot.cpp \
    serialdialog.cpp

HEADERS += \
    formchart.h \
    formgroup.h \
    formouttable.h \
    mainwindow.h \
    modbusdialog.h \
    outdialog.h \
    qcustomplot.h \
    serialdialog.h

FORMS += \
    formchart.ui \
    formgroup.ui \
    formouttable.ui \
    mainwindow.ui \
    modbusdialog.ui \
    outdialog.ui \
    serialdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    logo.qrc

RC_ICONS = logo.ico

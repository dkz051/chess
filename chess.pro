#-------------------------------------------------
#
# Project created by QtCreator 2019-08-26T14:03:31
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = chess
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

CONFIG += c++17

SOURCES += \
        controller.cpp \
        frmpromote.cpp \
        frmstart.cpp \
        global.cpp \
        graphics.cpp \
        main.cpp \
        frmmain.cpp

HEADERS += \
        controller.h \
        frmmain.h \
        frmpromote.h \
        frmstart.h \
        global.h \
        graphics.h

FORMS += \
        frmmain.ui \
        frmpromote.ui \
        frmstart.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
	chess.qrc

RC_ICONS += \
	chess.ico

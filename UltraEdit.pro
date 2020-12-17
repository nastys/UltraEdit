#-------------------------------------------------
#
# Project created by QtCreator 2020-08-10T04:18:43
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = UltraEdit
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
        convert.cpp \
        dtmgr.cpp \
    dtmgr_settings.cpp \
        glmodule.cpp \
        main.cpp \
        mainwindow.cpp \
        module.cpp \
        modulespr.cpp \
    psp-save/chnnlsv.c \
    psp-save/hash.c \
    psp-save/libkirk/AES.c \
    psp-save/libkirk/SHA1.c \
    psp-save/libkirk/amctrl.c \
    psp-save/libkirk/bn.c \
    psp-save/libkirk/ec.c \
    psp-save/libkirk/kirk_engine.c \
    psp-save/psf.c \
    psp-save/pspcrypt.c \
    pvdbparse.cpp \
        qgzdec.cpp \
        qt-crc32/crc32.cpp \
        savegen.cpp

HEADERS += \
        configfile.h \
        constants.h \
        convert.h \
        data.h \
        dtmgr.h \
        dtmgr_settings.h \
        glmodule.h \
        mainwindow.h \
        module.h \
        modulespr.h \
        psp-save/chnnlsv.h \
        psp-save/hash.h \
        psp-save/libkirk/AES.h \
        psp-save/libkirk/SHA1.h \
        psp-save/libkirk/amctrl.h \
        psp-save/libkirk/kirk_engine.h \
        psp-save/psf.h \
        psp-save/pspcrypt.h \
        pvdbparse.h \
        qgzdec.h \
        qt-crc32/crc32.h \
        savegen.h \
        types.h

FORMS += \
        dtmgr.ui \
        dtmgr_settings.ui \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    2dtex.vert \
    ati2ycbcr.frag \
    psp-save/CMakeLists.txt \
    psp-save/COPYING \
    psp-save/README.md \
    qt-crc32/LICENSE \
    qt-crc32/README.md \
    rgba8.frag

LIBS += -lz

RESOURCES += \
    shader.qrc

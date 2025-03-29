QT       += core gui
QMAKE_LFLAGS += -Wl,--verbose

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20

LIBS += -L$$PWD/../../DeviceManagerLibMingw/build-DeviceManagerLib-Desktop_Qt_6_4_1_MinGW_64_bit-Debug/debug/ -lDeviceManagerLib
win32:LIBS += -lhid -lsetupapi -lpdh -lnewdev -lwbemuuid -lcfgmgr32


SOURCES += \
    DeviceControlWidget.cpp \
    DeviceTreeWidget.cpp \
    PerformanceWidget.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    DeviceControlWidget.h \
    DeviceTreeWidget.h \
    PerformanceWidget.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

INCLUDEPATH += $$PWD/../../DeviceManagerLibMingw/DeviceManagerLib/

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

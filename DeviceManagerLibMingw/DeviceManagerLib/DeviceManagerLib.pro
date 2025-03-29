QT -= gui

TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++20

win32:LIBS += -lhid -lsetupapi -lpdh -lnewdev -lwbemuuid -lcfgmgr32

SOURCES += \
    DeviceEnumerator.cpp \
    DeviceController.cpp \
    DeviceDiagnostic.cpp \
    DeviceInfo.cpp \
    DeviceTreeNode.cpp \
    PerformanceMonitor.cpp \
    pch.cpp

HEADERS += \
    DeviceEnumerator.h \
    DeviceController.h \
    DeviceDiagnostic.h \
    DeviceInfo.h \
    DeviceTreeNode.h \
    PerformanceMonitor.h \
    pch.h \
    DeviceManagerLib.h


unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target


HEADERS  += $$PWD/src/core/qxtglobal.h
SOURCES  += $$PWD/src/core/qxtglobal.cpp

HEADERS  += $$PWD/src/gui/qxtglobalshortcut.h \
        $$PWD/src/gui/qxtglobalshortcut_p.h \
        $$PWD/src/gui/qxtwindowsystem.h

SOURCES += $$PWD/src/gui/qxtglobalshortcut.cpp \
        $$PWD/src/gui/qxtwindowsystem.cpp

unix:!macx {
        CONFIG += x11
        SOURCES += $$PWD/src/gui/qxtglobalshortcut_x11.cpp \
                $$PWD/src/gui/qxtwindowsystem_x11.cpp
}
macx {
        SOURCES += $$PWD/src/gui/qxtglobalshortcut_mac.cpp \
                $$PWD/src/gui/qxtwindowsystem_mac.cpp

        HEADERS  += $$PWD/src/gui/qxtwindowsystem_mac.h

        QMAKE_LFLAGS += -framework Carbon -framework CoreFoundation
}
win32 {
        SOURCES += $$PWD/src/gui/qxtglobalshortcut_win.cpp \
                $$PWD/src/gui/qxtwindowsystem_win.cpp
}


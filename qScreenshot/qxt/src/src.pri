HEADERS  += $$PWD/core/qxtglobal.h
SOURCES  += $$PWD/core/qxtglobal.cpp

HEADERS  += $$PWD/gui/qxtglobalshortcut.h \
	$$PWD/gui/qxtglobalshortcut_p.h \
	$$PWD/gui/qxtwindowsystem.h

SOURCES += $$PWD/gui/qxtglobalshortcut.cpp \
	$$PWD/gui/qxtwindowsystem.cpp

unix:!macx {
	CONFIG += X11
	SOURCES += $$PWD/gui/qxtglobalshortcut_x11.cpp \
		$$PWD/gui/qxtwindowsystem_x11.cpp
}
macx {
	SOURCES += $$PWD/gui/qxtglobalshortcut_mac.cpp \
		$$PWD/gui/qxtwindowsystem_mac.cpp

	HEADERS  += $$PWD/gui/qxtwindowsystem_mac.h

	QMAKE_LFLAGS += -framework Carbon -framework CoreFoundation
}
win32 {
	SOURCES += $$PWD/gui/qxtglobalshortcut_win.cpp \
		$$PWD/gui/qxtwindowsystem_win.cpp
}

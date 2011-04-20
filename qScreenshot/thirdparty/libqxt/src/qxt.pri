HEADERS  += $$PWD/core/qxtglobal.h
SOURCES  += $$PWD/core/qxtglobal.cpp

HEADERS  += $$PWD/gui/qxtglobalshortcut.h
HEADERS  += $$PWD/gui/qxtglobalshortcut_p.h
SOURCES += $$PWD/gui/qxtglobalshortcut.cpp

unix:!macx {
	SOURCES += $$PWD/gui/qxtglobalshortcut_x11.cpp
}
macx {
	SOURCES += $$PWD/gui/qxtglobalshortcut_mac.cpp
}
win32 {
	SOURCES += $$PWD/gui/qxtglobalshortcut_win.cpp
}

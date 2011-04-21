TEMPLATE = app
CONFIG += qt release
QT += network
TARGET = qScreenshot

DESTDIR = release/
MOC_DIR = tmp/
OBJECTS_DIR = tmp/

INCLUDEPATH += ../qxt/src/gui \
		../qxt/src/core

DEPENDPATH += ../qxt/release \
		../qxt/src/core \
		../qxt/src/gui

include(../conf.pri)

LIBS += -L../qxt/release/ -lqxt


HEADERS += $$PWD/screenshot.h \
    $$PWD/server.h \
    $$PWD/editserverdlg.h \
    $$PWD/screenshotoptions.h \
    $$PWD/toolbar.h \
    $$PWD/pixmapwidget.h \
    $$PWD/options.h \
    $$PWD/optionsdlg.h \
    $$PWD/optionswidget.h \
    $$PWD/iconset.h \
    $$PWD/controller.h \
    $$PWD/defines.h \
    $$PWD/proxysettingsdlg.h \
    $$PWD/shortcutmanager.h
SOURCES += $$PWD/screenshot.cpp \
    $$PWD/server.cpp \
    $$PWD/editserverdlg.cpp \
    $$PWD/screenshotoptions.cpp \
    $$PWD/toolbar.cpp \
    $$PWD/pixmapwidget.cpp \
    $$PWD/options.cpp \
    $$PWD/optionsdlg.cpp \
    $$PWD/optionswidget.cpp \
    $$PWD/iconset.cpp \
    $$PWD/controller.cpp \
    $$PWD/proxysettingsdlg.cpp \
    $$PWD/main.cpp \
    $$PWD/shortcutmanager.cpp
FORMS += $$PWD/optionswidget.ui \
    $$PWD/editserverdlg.ui \
    $$PWD/screenshot.ui \
    $$PWD/screenshotoptions.ui \
    $$PWD/optionsdlg.ui \
    $$PWD/proxysettingsdlg.ui

RESOURCES += screenshot.qrc

unix {
	target.path=$$BINDIR
	INSTALLS += target

	dt.path=$$PREFIX/share/applications/
	dt.files = ../qscreenshot.desktop
	icon.path=$$PREFIX/share/icons/hicolor/16x16/apps
	icon.files = icons/screenshot.png

	INSTALLS += dt icon
}

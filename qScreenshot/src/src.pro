TEMPLATE = app

CONFIG += qt \
    release
QT += network

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
    DEFINES += HAVE_QT5
}
unix:!mac:DEFINES += HAVE_X11

TARGET = qScreenshot
DESTDIR = release/
MOC_DIR = tmp/
OBJECTS_DIR = tmp/
UI_DIR = tmp/

INCLUDEPATH += . \
    ../qxt/src/gui \
    ../qxt/src/core
DEPENDPATH += . \
    ../qxt/src/core \
    ../qxt/src/gui

include(../conf.pri)
include(../qxt/qxt.pro)

HEADERS += $$PWD/screenshotmainwin.h \
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
    $$PWD/shortcutmanager.h \
    $$PWD/aboutdlg.h \
    $$PWD/translator.h \
    $$PWD/updateschecker.h \
    $$PWD/historydlg.h \
    $$PWD/grabareawidget.h \
    $$PWD/screenshoter.h \
    $$PWD/uploader.h \
    $$PWD/common.h
SOURCES += $$PWD/screenshotmainwin.cpp \
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
    $$PWD/shortcutmanager.cpp \
    $$PWD/aboutdlg.cpp \
    $$PWD/translator.cpp \
    $$PWD/updateschecker.cpp \
    $$PWD/historydlg.cpp \
    $$PWD/grabareawidget.cpp \
    $$PWD/screenshoter.cpp \
    $$PWD/uploader.cpp \
    $$PWD/common.cpp
FORMS += $$PWD/optionswidget.ui \
    $$PWD/editserverdlg.ui \
    $$PWD/screenshot.ui \
    $$PWD/screenshotoptions.ui \
    $$PWD/optionsdlg.ui \
    $$PWD/proxysettingsdlg.ui \
    $$PWD/aboutdlg.ui
RESOURCES += screenshot.qrc

win32:RC_FILE += screenshot.rc

isEmpty(PREFIX) {
        PREFIX = /usr/local
}
BINDIR = $$PREFIX/bin
DATADIR = $$PREFIX/share/qscreenshot

DEFINES += QSCREENSHOT_DATADIR='\\"$$DATADIR\\"'

LANG_PATH = $$PWD/lang
TRANSLATIONS = $$LANG_PATH/ru.ts

unix { 
    target.path = $$BINDIR
    INSTALLS += target
    dt.path = $$PREFIX/share/applications/
    dt.files = ../qscreenshot.desktop
    icon1.path = $$PREFIX/share/icons/hicolor/16x16/apps
    icon1.files = icons/logo/screenshot_16.png
    icon2.path = $$PREFIX/share/icons/hicolor/22x22/apps
    icon2.files = icons/logo/screenshot_22.png
    icon3.path = $$PREFIX/share/icons/hicolor/24x24/apps
    icon3.files = icons/logo/screenshot_24.png
    icon4.path = $$PREFIX/share/icons/hicolor/32x32/apps
    icon4.files = icons/logo/screenshot_32.png
    icon5.path = $$PREFIX/share/icons/hicolor/48x48/apps
    icon5.files = icons/screenshot.png

    translations.path = $$DATADIR/translations
    LRELEASE = "lrelease"
    !exists($$LRELEASE)
        LRELEASE = "lrelease-qt4"
    translations.extra = $$LRELEASE src.pro && cp -f $$LANG_PATH/*.qm  $(INSTALL_ROOT)$$translations.path
    INSTALLS += translations

    INSTALLS += dt \
        icon1 \
        icon2 \
        icon3 \
        icon4 \
        icon5
}
macx {
   ICON = icons/logo/screenshot.icns
   QMAKE_INFO_PLIST = ../Info.plist
}

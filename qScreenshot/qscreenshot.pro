include(src/src.pri)
include(thirdparty/libqxt/src/qxt.pri)
INCLUDEPATH += src/ \
		thirdparty/libqxt/src/gui \
		thirdparty/libqxt/src/core
CONFIG += qt release
QT += network
RESOURCES += screenshot.qrc
DESTDIR = release/
MOC_DIR = tmp/
OBJECTS_DIR = tmp/


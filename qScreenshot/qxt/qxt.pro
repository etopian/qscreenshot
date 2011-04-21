TEMPLATE = lib

INCLUDEPATH += src/gui/ \
		src/core/

DESTDIR = release/
MOC_DIR = tmp/
OBJECTS_DIR = tmp/

include(src/src.pri)

unix {
	target.path=/usr/lib
	INSTALLS += target
}

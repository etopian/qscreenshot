#TEMPLATE = lib
#CONFIG += -debug release staticlib
INCLUDEPATH += src/gui/ \
		src/core/

#DESTDIR = release/
MOC_DIR = tmp/
OBJECTS_DIR = tmp/

include(src/src.pri)

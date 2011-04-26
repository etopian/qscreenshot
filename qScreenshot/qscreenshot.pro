TEMPLATE = subdirs

include(conf.pri)

#sub_qxt.subdir = qxt
sub_src.subdir = src
#sub_src.depends = sub_qxt

SUBDIRS += sub_src





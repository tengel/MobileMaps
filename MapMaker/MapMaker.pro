# -------------------------------------------------
# Project created by QtCreator 2008-11-11T18:31:27
# -------------------------------------------------
TARGET = MapMaker
TEMPLATE = app
SOURCES += main.cpp \
    dialog.cpp \
    progressdialog.cpp \
    maker.cpp \
    zlib/uncompr.c \
    zlib/deflate.c \
    zlib/inflate.c \
    zlib/zutil.c \
    zlib/infback.c \
    zlib/inffast.c \
    zlib/crc32.c \
    zlib/adler32.c \
    zlib/compress.c \
    zlib/trees.c \
    zlib/inftrees.c \
    zlib/gzio.c \
    zlib/minizip/unzip.c \
    zlib/minizip/zip.c \
    zlib/minizip/mztools.c \
    zlib/minizip/ioapi.c \
    index.cpp \
    pngcrush.cpp \
    zipfile.cpp \
    indexeditor.cpp \
    indexwidget.cpp \
    indexitem.cpp \
    mainwindow.cpp
HEADERS += dialog.h \
    progressdialog.h \
    maker.h \
    zlib/zlib.h \
    zlib/deflate.h \
    zlib/zconf.h \
    zlib/trees.h \
    zlib/inffixed.h \
    zlib/crc32.h \
    zlib/inftrees.h \
    zlib/inffast.h \
    zlib/zutil.h \
    zlib/inflate.h \
    zlib/minizip/mztools.h \
    zlib/minizip/unzip.h \
    zlib/minizip/zip.h \
    zlib/minizip/ioapi.h \
    zlib/minizip/crypt.h \
    util.h \
    index.h \
    pngcrush.h \
    zipfile.h \
    indexeditor.h \
    indexwidget.h \
    indexitem.h \
    mainwindow.h
FORMS += dialog.ui \
    progressdialog.ui \
    indexeditor.ui \
    mainwindow.ui

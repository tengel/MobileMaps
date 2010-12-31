# -------------------------------------------------
# Project created by QtCreator 2009-03-24T21:48:21
# -------------------------------------------------
QT += network \
    webkit
TARGET = StreetmapCreator
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    osmview.cpp \
    progressdialog.cpp \
    Way.cpp \
    Transform.cpp \
    Index.cpp \
    osmparser.cpp \
    Node.cpp \
    StreetmapDownloader.cpp \
    ScaleDialog.cpp \
    PngCrush.cpp \
    MapWriter.cpp \
    FileStoreWriter.cpp \
    IndexBuilder.cpp
HEADERS += mainwindow.h \
    osmview.h \
    progressdialog.h \
    Way.h \
    Util.h \
    Transform.h \
    Index.h \
    osmparser.h \
    Node.h \
    StreetmapDownloader.h \
    ScaleDialog.h \
    TileID.h \
    PngCrush.h \
    MapWriter.h \
    FileStoreWriter.h \
    IndexBuilder.h
FORMS += mainwindow.ui \
    progressdialog.ui \
    ScaleDialog.ui
RESOURCES += resources.qrc

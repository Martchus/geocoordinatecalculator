projectname = geocoordinatecalculator
appname = "Geo coordinate calculator"
appauthor = Martchus
appurl = "https://github.com/$${appauthor}/$${projectname}"
QMAKE_TARGET_DESCRIPTION = "Command line tool for basic calculations with geo coordinates such as format conversions and calculation of distance, bearing, mid point, destination and more."
VERSION = 1.0.1

CONFIG += no-gui

# include ../../common.pri when building as part of a subdirs project; otherwise include general.pri
!include(../../common.pri) {
    !include(./general.pri) {
        error("Couldn't find the common.pri or the general.pri file!")
    }
}

TEMPLATE = app

CONFIG -= qt
CONFIG += console

SOURCES += main.cpp \
    location.cpp \
    angle.cpp \
    utils.cpp

HEADERS += \
    main.h \
    location.h \
    angle.h \
    utils.h

RESOURCES += images.qrc

OTHER_FILES += \
    README.md \
    LICENSE

CONFIG(debug, debug|release) {
    LIBS += -lc++utilitiesd
} else {
    LIBS += -lc++utilities
}

# installs
target.path = $$(INSTALL_ROOT)/bin
INSTALLS += target

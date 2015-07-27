projectname = geocoordinatecalculator
appname = "Geo coordinate calculator"
appauthor = Martchus
appurl = "https://github.com/$${appauthor}/$${projectname}"
VERSION = 1.0.0

CONFIG += no-gui

# include ../../common.pri when building as part of a subdirs project; otherwise include general.pri
!include(../../common.pri) {
    !include(./general.pri) {
        error("Couldn't find the common.pri or the general.pri file!")
    }
}

TEMPLATE = app

CONFIG -= qt

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
       LIBS    += -L../../ -lc++utilitiesd
} else {
       LIBS    += -L../../ -lc++utilities
}

INCLUDEPATH += ../

# installs
target.path = $$(INSTALL_ROOT)/bin
INSTALLS += target

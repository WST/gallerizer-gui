DEFINES += INSTALL_PREFIX=$$(PREFIX)
TRANSLATIONS += gallerizer_ru.ts gallerizer_en.ts
CODECFORTR = UTF-8
QT += core gui
TARGET = gallerizer
TEMPLATE = app
SOURCES += main.cpp window.cpp
HEADERS  += window.h
FORMS    += window.ui
RESOURCES += resources.qrc
win32:RC_FILE = gallerizer.rc

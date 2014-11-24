QT         += core gui network xml consle
TARGET      = xpenser
TEMPLATE    = app
SOURCES    += main.cpp\
              mainwindow.cpp \
              httprequest.cpp \
    xmlhandler.cpp
HEADERS    += mainwindow.h \
              httprequest.h \
    xmlhandler.h \
    typedef.h
FORMS      += authenticationdialog.ui
CODECFORTR = UTF-8

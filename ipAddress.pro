#-------------------------------------------------
#
# Project created by QtCreator 2012-08-28T22:34:36
#
#-------------------------------------------------

QT       += core
QT       += network

QT       -= gui

TARGET = ipAddress
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    other/smtpclient.cpp \
    other/quotedprintable.cpp \
    other/mimetext.cpp \
    other/mimepart.cpp \
    other/mimemultipart.cpp \
    other/mimemessage.cpp \
    other/mimeinlinefile.cpp \
    other/mimehtml.cpp \
    other/mimefile.cpp \
    other/mimecontentformatter.cpp \
    other/mimeattachment.cpp \
    other/emailaddress.cpp

HEADERS += \
    other/SmtpMime \
    other/smtpclient.h \
    other/quotedprintable.h \
    other/mimetext.h \
    other/mimepart.h \
    other/mimemultipart.h \
    other/mimemessage.h \
    other/mimeinlinefile.h \
    other/mimehtml.h \
    other/mimefile.h \
    other/mimecontentformatter.h \
    other/mimeattachment.h \
    other/emailaddress.h

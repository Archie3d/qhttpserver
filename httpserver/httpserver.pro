#
#                         qhttpserver
#
#   Copyright (C) 2015 Arthur Benilov,
#   arthur.benilov@gmail.com
#
#   This program is free software; you can redistribute it and/or
#   modify it under the terms of the GNU Lesser General Public
#   License as published by the Free Software Foundation; either
#   version 2.1 of the License, or (at your option) any later version.
#
#   This software is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
#   Lesser General Public License for more details.

TEMPLATE = lib

QT       += core network
QT       -= gui

CONFIG += dll

TARGET = httpserver
DEFINES += HTTPSERVERLIB_BUILD

SOURCES += \
    HttpServer.cpp \
    HttpClientHandler.cpp \
    HttpThreadPool.cpp \
    HttpRequest.cpp \
    HttpOption.cpp \
    HttpContentType.cpp \
    HttpRequestHandler.cpp \
    HttpRequestRouter.cpp \
    HttpFileRequestHandler.cpp \
    HttpResponse.cpp

HEADERS += \
    HttpServerApi.h \
    HttpServer.h \
    HttpClientHandler.h \
    HttpThreadPool.h \
    HttpRequest.h \
    HttpOption.h \
    HttpContentType.h \
    HttpResponse.h \
    HttpRequestHandler.h \
    HttpRequestRouter.h \
    HttpFileRequestHandler.h \
    IHttpClientHandler.h

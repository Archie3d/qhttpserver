/*
                          qhttpserver

    Copyright (C) 2015 Arthur Benilov,
    arthur.benilov@gmail.com

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This software is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    Lesser General Public License for more details.
*/

#ifndef HTTPREQUESTHANDLER_H
#define HTTPREQUESTHANDLER_H

#include <QObject>
#include "HttpRequest.h"
#include "HttpResponse.h"

/**
 * Abstract handler of HTTP requests.
 */
class HttpRequestHandler : public QObject
{
    Q_OBJECT

public:

    explicit HttpRequestHandler(QObject *pParent = nullptr);

    /**
     * Handle HTTP request.
     * @param request Incoming request.
     * @param response Generated response.
     */
    virtual void handleRequest(const HttpRequest &request, HttpResponse &response) = 0;

public slots:

    void processRequest(const HttpRequest &request, HttpResponse &response);

};

#endif // HTTPREQUESTHANDLER_H


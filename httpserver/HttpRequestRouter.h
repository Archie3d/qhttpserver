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

#ifndef HTTPREQUESTROUTER_H
#define HTTPREQUESTROUTER_H

#include <QMutex>
#include "HttpServerApi.h"
#include "HttpRequestHandler.h"

class HTTP_API HttpRequestRouter : public HttpRequestHandler
{
    Q_OBJECT
public:
    explicit HttpRequestRouter(QObject *pParent = nullptr);
    ~HttpRequestRouter();

    HttpRequestRouter& map(const QRegExp &pattern, HttpRequestHandler *pHandler);
    HttpRequestRouter& unmap(HttpRequestHandler *pHandler);

    /**
     * Remove all handlers.
     */
    void clear();

    void handleRequest(const HttpRequest &request, HttpResponse &response);

private slots:

    void onHandlerDeleted(QObject *pObject);

private:

    void unmapUnsafe(HttpRequestHandler *pHandler);
    HttpRequestHandler* findRequestHandler(const HttpRequest &request);

    QMutex m_mutex; ///< Protective mutex.

    /// URL pattern to handler pair.
    typedef QPair<QRegExp, HttpRequestHandler*> PatternHandlerPair;

    /// List of request handlers.
    QList<PatternHandlerPair> m_handlers;
};

#endif // HTTPREQUESTROUTER_H


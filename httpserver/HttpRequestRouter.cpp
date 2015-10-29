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

#include "HttpRequestRouter.h"

HttpRequestRouter::HttpRequestRouter(QObject *pParent)
    : HttpRequestHandler(pParent),
      m_mutex(),
      m_handlers()
{
}

HttpRequestRouter::~HttpRequestRouter()
{
}

HttpRequestRouter& HttpRequestRouter::map(const QRegExp &pattern, HttpRequestHandler *pHandler)
{
    Q_ASSERT(pHandler != nullptr);

    QMutexLocker locker(&m_mutex);
    if (pHandler != nullptr) {
        m_handlers.append(PatternHandlerPair(pattern, pHandler));
        disconnect(pHandler, 0, this, 0);
        connect(pHandler, SIGNAL(destroyed(QObject*)), this, SLOT(onHandlerDeleted(QObject*)));
    }
    return *this;
}

HttpRequestRouter& HttpRequestRouter::unmap(HttpRequestHandler *pHandler)
{
    Q_ASSERT(pHandler != nullptr);

    QMutexLocker locker(&m_mutex);
    if (pHandler != nullptr) {
        unmapUnsafe(pHandler);
    }

    return *this;
}

void HttpRequestRouter::clear()
{
    QMutexLocker locker(&m_mutex);
    m_handlers.clear();
}

void HttpRequestRouter::handleRequest(const HttpRequest &request, HttpResponse &response)
{
    HttpRequestHandler *pHandler = findRequestHandler(request);
    if (pHandler == nullptr) {
        response.setStatus(HttpResponse::NotFound);
        return;
    }

    // Handler is executed in the client's handler thread.
    pHandler->handleRequest(request, response);
}

void HttpRequestRouter::onHandlerDeleted(QObject *pObject)
{
    QMutexLocker locker(&m_mutex);

    HttpRequestHandler *pHandler = qobject_cast<HttpRequestHandler*>(pObject);
    if (pHandler != nullptr) {
        unmapUnsafe(pHandler);
    }
}

void HttpRequestRouter::unmapUnsafe(HttpRequestHandler *pHandler)
{
    Q_ASSERT(pHandler != nullptr);

    QMutableListIterator<PatternHandlerPair> it(m_handlers);
    while (it.hasNext()) {
        PatternHandlerPair h = it.next();
        if (h.second == pHandler) {
            disconnect(pHandler, 0, this, 0);
            it.remove();
        }
    }
}

HttpRequestHandler* HttpRequestRouter::findRequestHandler(const HttpRequest &request)
{
    QString url(request.uri());

    QMutexLocker locker(&m_mutex);

    foreach (const PatternHandlerPair &p, m_handlers) {
        QRegExp regExp = p.first;

        if (regExp.indexIn(url) != -1) {
            HttpRequestHandler *pHandler = p.second;
            Q_ASSERT(pHandler != nullptr);
            return pHandler;
        }
    }

    // No handler found.
    return nullptr;
}

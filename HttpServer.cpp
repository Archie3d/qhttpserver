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

#ifdef _DEBUG
#   include <QDebug>
#endif
#include <QTcpSocket>
#include "HttpThreadPool.h"
#include "HttpClientHandler.h"
#include "HttpRequestRouter.h"
#include "HttpServer.h"

HttpServer::HttpServer(const QHostAddress &address, quint16 port, QObject *pParent)
    : QTcpServer(pParent),
      m_hostAddress(address),
      m_port(port),
      //m_pThreadPool(new HttpThreadPool(5, 10, this)),
      m_pThreadPool(nullptr),
      m_handlers(),
      m_pRequestRouter(new HttpRequestRouter(this))
{
}

HttpServer::~HttpServer()
{
    qDeleteAll(m_handlers);
}

void HttpServer::start()
{
    if (isListening()) {
        // Already started
        return;
    }

#ifdef _DEBUG
    qDebug() << "Starting server" << m_hostAddress.toString() << ":" << m_port;
#endif
    listen(m_hostAddress, m_port);
}


void HttpServer::stop()
{
    close();
#ifdef _DEBUG
    qDebug() << "Server stopped.";
#endif
}

void HttpServer::incomingConnection(qintptr handle)
{
    // Pass client socket to handler
    HttpClientHandler *pHandler = new HttpClientHandler(handle, m_pRequestRouter);

    // Move handler to dedicated thread if thread-pooling is used.
    if (m_pThreadPool != nullptr) {
        QThread *pThread = m_pThreadPool->allocate();
        if (pThread == nullptr) {
            delete pHandler;
        } else {
            pHandler->moveToThread(pThread);
        }
    }

    m_handlers.append(pHandler);
    connect(pHandler, SIGNAL(over(QObject*)), this, SLOT(onHandlerOver(QObject*)), Qt::QueuedConnection);
}

void HttpServer::onHandlerOver(QObject *pObject)
{
    Q_ASSERT(pObject != nullptr);

    HttpClientHandler *pHandler = qobject_cast<HttpClientHandler*>(pObject);
    if (m_handlers.contains(pHandler)) {
        // Release handler's thread
        if (m_pThreadPool != nullptr) {
            m_pThreadPool->putBack(pObject->thread());
        }
        m_handlers.removeOne(pHandler);
        disconnect(pHandler, 0, this, 0);
        pObject->deleteLater();
    }
}

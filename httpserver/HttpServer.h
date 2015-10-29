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

#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QThreadPool>
#include <QHostAddress>
#include <QTcpServer>
#include "HttpServerApi.h"

class HttpThreadPool;
class HttpClientHandler;
class HttpRequestRouter;

/**
 * Server class.
 * Handles incomming connections and dispatches them to
 * client handlers.
 */
class HTTP_API HttpServer : public QTcpServer
{
    Q_OBJECT
public:

    /**
     * Construct HTTP server but do not start it yet.
     * @param address Host address for incoming connections.
     * @param port TCP port number.
     * @param pParent Pointer to parent object.
     */
    HttpServer(const QHostAddress &address = QHostAddress::Any,
               quint16 port = 8000,
               QObject *pParent = nullptr);
    ~HttpServer();

    const QHostAddress& hostAddress() const { return m_hostAddress; }
    quint16 port() const { return m_port; }

    HttpRequestRouter* requestRouter() const { return m_pRequestRouter; }

public slots:

    /**
     * Start the server.
     */
    void start();

    /**
     * Stop the server.
     */
    void stop();

protected:

    void incomingConnection(qintptr handle);

private slots:

    void onHandlerOver(QObject *pObject);

private:
    Q_DISABLE_COPY(HttpServer)

    /// Host address used to listen for incoming connections.
    QHostAddress m_hostAddress;
    /// Server's TCP port number.
    quint16 m_port;
    /// Pool of threads used to dispatch client handlers.
    HttpThreadPool *m_pThreadPool;
    /// List of active handlers.
    QList<HttpClientHandler*> m_handlers;
    /// Handler of HTTP requests (by routing them to specific handlers).
    HttpRequestRouter *m_pRequestRouter;
};

#endif // HTTPSERVER_H

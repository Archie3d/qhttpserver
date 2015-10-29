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


#ifndef HTTPCLIENTHANDLER_H
#define HTTPCLIENTHANDLER_H

#include <QObject>
#include "HttpServerApi.h"
#include "IHttpClientHandler.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

class QTcpSocket;
class HttpRequestHandler;

class HTTP_API HttpClientHandler : public QObject, public IHttpClientHandler
{
    Q_OBJECT
public:

    /// Client handler state
    enum State {
        State_Ready,                    ///< Initial state.
        State_ReceiveRequest,           ///< Waiting for incoming request.
        State_ReceiveRequestOptions,    ///< Receive HTTP options.
        State_ReceiveRequestStringData, ///< Receive string-type data.
        State_ReceiveRequestBinaryData, ///< Receive binary-encoded data.
        State_ProcessRequest,           ///< Processing received request.
        State_WaitFinishResponse,       ///< Wait for response finalization.
        State_FinishResponse,           ///< Finalize response.
        State_CloseClient               ///< Close communication.
    };

    HttpClientHandler(qintptr handle, HttpRequestHandler *pRequestHandler, QObject *pParent = nullptr);
    ~HttpClientHandler();

    void finalizeResponse();

    bool isKeepAlive() const { return m_keepAlive; }
    void setKeepAlive(bool v) { m_keepAlive = v; }

public slots:

    /**
     * Close communication channel.
     */
    void close();

signals:

    void over(QObject*);

private slots:

    /// Handle socket disconnection.
    void onDisconnect();
    /// Handle incoming data.
    void onDataAvailable();

    void receiveRequest();
    void receiveOptions();
    void receiveStringData();
    void receiveBinaryData();
    void processRequest();
    void waitFinishResponse();
    void finishResponse();
    void closeClient();

    /// Handle current state.
    void handleState();
    void handleStateAsync();

private:

    void setState(State s, bool scheduleUpdate = false);

    State m_state;

    QTcpSocket *m_pSocket;
    bool m_keepAlive;

    HttpRequest m_request;      ///< Received request.
    HttpResponse m_response;    ///< Response to be sent back.
    qint64 m_contentReceived;   ///< Content length (received so far).
    qint64 m_contentLength;     ///< Content length (in bytes).

    HttpRequestHandler *m_pRequestHandler;
};

#endif // HTTPCLIENTHANDLER_H

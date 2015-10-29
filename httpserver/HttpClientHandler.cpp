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

#include <QMap>
#include <QTcpSocket>
#include <QFile>
#include "HttpRequest.h"
#include "HttpRequestHandler.h"
#include "HttpOption.h"
#include "HttpClientHandler.h"

static QMap<HttpClientHandler::State, QString> sStateToStringMap {
    {HttpClientHandler::State_Ready, "Ready"},
    {HttpClientHandler::State_ReceiveRequest, "ReceiveRequest"},
    {HttpClientHandler::State_ReceiveRequestOptions, "ReceiveRequestOptions"},
    {HttpClientHandler::State_ReceiveRequestStringData, "ReceiveRequestStringData"},
    {HttpClientHandler::State_ReceiveRequestBinaryData, "ReceiveRequestBinaryDaya"},
    {HttpClientHandler::State_ProcessRequest, "ProcessRequest"},
    {HttpClientHandler::State_WaitFinishResponse, "WaitFinishResponse"},
    {HttpClientHandler::State_FinishResponse, "FinishResponse"},
    {HttpClientHandler::State_CloseClient, "CloseClient"}
};

HttpClientHandler::HttpClientHandler(qintptr handle, HttpRequestHandler *pRequestHandler, QObject *pParent)
    : QObject(pParent),
      m_state(State_Ready),
      m_pSocket(nullptr),
      m_keepAlive(false),
      m_request(),
      m_response(),
      m_contentReceived(0L),
      m_contentLength(0L),
      m_pRequestHandler(pRequestHandler)
{
    m_pSocket = new QTcpSocket(this);
    m_pSocket->setSocketDescriptor(handle);

    m_keepAlive = false;

    connect(m_pSocket, SIGNAL(disconnected()), this, SLOT(onDisconnect()));
    connect(m_pSocket, SIGNAL(readyRead()), this, SLOT(onDataAvailable()));
}

HttpClientHandler::~HttpClientHandler()
{
    // Socket is supposed to be closed here.
    // Disconnect all signals from socket.
    disconnect(m_pSocket, 0, this, 0);
    m_pSocket->abort();
}

void HttpClientHandler::finalizeResponse()
{
    Q_ASSERT(m_state == State_WaitFinishResponse);

    if (m_state == State_WaitFinishResponse) {
        setState(State_FinishResponse, true);
    }
}

void HttpClientHandler::close()
{
    // Disconnect signals from socket to avoid
    // duplications.
    disconnect(m_pSocket, 0, this, 0);
    m_pSocket->close();
    emit over(this);
}

void HttpClientHandler::onDisconnect()
{
    close();
}

void HttpClientHandler::onDataAvailable()
{
    while (m_pSocket->bytesAvailable() > 0) {
        handleState();
    }
}

void HttpClientHandler::receiveRequest()
{
    if (!m_pSocket->canReadLine()) {
        // Wait for more data
        return;
    }

    m_request = HttpRequest();
    m_response = HttpResponse(this);

    QString strRequest = m_pSocket->readLine();
    QStringList reqList = strRequest.split(" ", QString::SkipEmptyParts);
    if (reqList.count() < 3) {
        // Request is formed incorrectly
        return;
    }

    HttpRequest::Method method = HttpRequest::stringToMethod(reqList.at(0));
    if (method == HttpRequest::Method_Unknown ||
            method == HttpRequest::Method_Invalid) {
        // Unknown or invalid method
        return;
    }

    // Parse URI
    QByteArray ba = QByteArray::fromPercentEncoding(reqList.at(1).toUtf8());
    QString uri(ba);

    // Parse URI arguments
    QStringList urlArgs = uri.split("?", QString::SkipEmptyParts);
    if (urlArgs.count() > 1) {
        uri = urlArgs.at(0);
        QString strArgs = urlArgs.at(1);
        m_request.parseArguments(strArgs);
    }

    // Parse HTTP version
    QString httpVersion = reqList.at(2);
    if (httpVersion.left(5) != "HTTP/") {
        // Invalid version signature
        return;
    }
    QStringList versionList = httpVersion.mid(5).split(".", QString::SkipEmptyParts);
    bool ok = false;
    int major = versionList.at(0).toInt(&ok);
    if (!ok) {
        return;
    }
    int minor = versionList.at(1).toInt(&ok);
    if (!ok) {
        return;
    }

    m_request.setMethod(method);
    m_request.setUri(uri);
    m_request.setMajorVersion(major);
    m_request.setMinorVersion(minor);

#ifdef _DEBUG
    qDebug() << m_request.toString();
#endif

    // Proceed to the next state
    setState(State_ReceiveRequestOptions, true);
}

void HttpClientHandler::receiveOptions()
{
    while (m_pSocket->canReadLine()) {
        QString line = m_pSocket->readLine();
        if (line.trimmed().isEmpty()) {
            // Empty line received
            bool ok = false;
            m_contentReceived = 0L;
            m_contentLength = m_request.option(HttpOption::ContentLength).toLongLong(&ok);
            if (!ok) {
                // Content length is not provided
                m_contentLength = -1L;
            }

            if (m_contentLength > 0) {
                setState(State_ReceiveRequestBinaryData, true);
            } else {
                if (m_request.method() == HttpRequest::Method_Get) {
                    // No data to receive
                    setState(State_ProcessRequest, true);
                } else {
                    setState(State_ReceiveRequestStringData, true);
                }
            }
        } else {
            // Check for options
            int pos = line.indexOf(":");
            if (pos > 0) {
                QString key = line.left(pos).trimmed();
                QString value = line.mid(pos + 1).trimmed();
                if (key.toLower() == "cookie") {
                    // This option is a cookie
                    QStringList lstCookies = value.split(";", QString::SkipEmptyParts);
                    foreach (const QString &str, lstCookies) {
                        m_request.parseCookie(str);
                    }
                } else {
                    // Not a cookie
                    m_request.addOption(key, value);
                }
            }
        }
    }
}

void HttpClientHandler::receiveStringData()
{
    while (m_pSocket->canReadLine()) {
        QString line = m_pSocket->readLine();
        if (line.trimmed().isEmpty()) {
            // Data is over
            setState(State_ProcessRequest, true);
            return;
        }

        m_request.appendData(line.toLatin1());
    }
}

void HttpClientHandler::receiveBinaryData()
{
    qint64 available;
    while ((available = m_pSocket->bytesAvailable()) > 0) {
        qint64 toRead = m_contentLength - m_contentReceived;
        if (toRead > available) {
            toRead = available;
        }
        QByteArray ba = m_pSocket->read(toRead);
        m_request.appendData(ba);
        m_contentReceived += toRead;
        if (m_contentReceived >= m_contentLength) {
            // All data received
            setState(State_ProcessRequest, true);
            return;
        }
    }
}

void HttpClientHandler::processRequest()
{
    if (m_request.isValid()) {
        m_response.setVersion(m_request.majorVersion(), m_request.minorVersion());
        m_response.setSocket(m_pSocket);

        if (m_request.contentType() == HttpContentType::ApplicationFromUrlEncoded) {
            QString strData = QString(m_request.constData());
            m_request.parseArguments(strData);
            m_request.setData(QByteArray());
        }

        if (m_pRequestHandler != nullptr) {
            /* The state is set to 'waiting for response finalization' so that
             * the handler could finalize the response once it is ready.
             */
            setState(State_WaitFinishResponse);
            m_pRequestHandler->processRequest(m_request, m_response);
            return;
        } else {
            m_response.setStatus(HttpResponse::NotFound);
        }

    } else {
        // Invalid request
        m_response.setStatus(HttpResponse::BadRequest);
    }
    setState(State_FinishResponse, true);
}

void HttpClientHandler::waitFinishResponse()
{
}

void HttpClientHandler::finishResponse()
{
#ifdef _DEBUG
    qDebug() << m_response.status() << HttpResponse::statusToString(m_response.status());
#endif

    if (m_response.status() != HttpResponse::Invalid && (!m_response.isSent())) {
        if (m_response.status() != HttpResponse::Ok && m_response.constData().size() == 0) {
            // Serve an error page
            int err = static_cast<int>(m_response.status());
            m_response.setContentType(HttpContentType::TextHtml);
            m_response.setOption(HttpOption::Connection, "Close");

            // TODO: Move this to resource file
            QString html = QString(
                        "<html>"
                        "<head>"
                        "<title>%1-%2</title>"
                        "</head>"
                        "<body>"
                        "<h1>%1</h1>"
                        "<h3>%2</h3>"
                        "Requested resource %3 cannot be found on this server"
                        "</body>"
                        "</html>")
                    .arg(err)
                    .arg(HttpResponse::statusToString(m_response.status()))
                    .arg(m_request.uri());

            QByteArray data = html.toLatin1();

            m_response.setData(data);
        }
    }

    // Send the response if not yet sent.
    if (!m_response.isSent()) {
        m_response.send();
    }

    // Check for keep-alive option
    m_keepAlive = false;
    QString kaStr = m_request.option(HttpOption::Connection).toString().toLower();
    if (kaStr == "keep-alive") {
        m_keepAlive = (m_response.option(HttpOption::Connection).toString().toLower() != "close");
    }

    if (isKeepAlive()) {
        m_request = HttpRequest();
        m_response = HttpResponse();
        setState(State_ReceiveRequest, true);
    } else {
        setState(State_CloseClient, true);
    }
}

void HttpClientHandler::closeClient()
{
    // Close connection and notify the thread is over.
    close();
}

void HttpClientHandler::handleState()
{
    switch (m_state) {
    case State_Ready:
        setState(State_ReceiveRequest);
        // Fall down to consequent state handling
    case State_ReceiveRequest:
        receiveRequest();
        break;
    case State_ReceiveRequestOptions:
        receiveOptions();
        break;
    case State_ReceiveRequestStringData:
        receiveStringData();
        break;
    case State_ReceiveRequestBinaryData:
        receiveBinaryData();
        break;
    case State_ProcessRequest:
        processRequest();
        break;
    case State_FinishResponse:
        finishResponse();
        break;
    case State_CloseClient:
        closeClient();
        break;
    default:
        Q_ASSERT(!"Invalid state");
        break;
    }
}

void HttpClientHandler::handleStateAsync()
{
    QMetaObject::invokeMethod(this, "handleState");
}

void HttpClientHandler::setState(State s, bool scheduleUpdate)
{
#ifdef _DEBUG
    //qDebug() << sStateToStringMap[m_state] << "-->" << sStateToStringMap[s];
#endif

    m_state = s;
    if (scheduleUpdate) {
        handleStateAsync();
    }
}

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

#include "HttpOption.h"
#include "HttpResponse.h"

static QMap<HttpResponse::Status, QString> sStatusToReasonMap {
    {HttpResponse::Continue, "Continue"},
    {HttpResponse::SwitchingProtocols, "Switching Protocols"},
    {HttpResponse::Ok, "OK"},
    {HttpResponse::Created, "Created"},
    {HttpResponse::Accepted, "Accepted"},
    {HttpResponse::NonAuthoritativeInformation, "Non Authoritative Information"},
    {HttpResponse::NoContent, "No Content"},
    {HttpResponse::ResetContent, "Reset Content"},
    {HttpResponse::PartialContent, "Partial Content"},
    {HttpResponse::MultipleChoices, "Multiple Choices"},
    {HttpResponse::MovedPermanently, "Moved Permanently"},
    {HttpResponse::Found, "Found"},
    {HttpResponse::SeeOther, "See Other"},
    {HttpResponse::NotModified, "Not Modified"},
    {HttpResponse::UseProxy, "Use Proxy"},
    {HttpResponse::TemporaryRedirect, "Temporary Redirect"},
    {HttpResponse::BadRequest, "Bad Request"},
    {HttpResponse::Unauthorized, "Unauthorized"},
    {HttpResponse::PaymentRequered, "Payment Required"},
    {HttpResponse::Forbidden, "Forbidden"},
    {HttpResponse::NotFound, "Not Found"},
    {HttpResponse::MethodNotAllowed, "Method Not Allowed"},
    {HttpResponse::NotAcceptable, "Not Acceptable"},
    {HttpResponse::ProxyAuthenticationRequired, "Proxy Authentication Required"},
    {HttpResponse::RequestTimeOut, "Request Time Out"},
    {HttpResponse::Conflict, "Conflict"},
    {HttpResponse::Gone, "Gone"},
    {HttpResponse::LengthRequired, "Length Required"},
    {HttpResponse::PreconditionFailed, "Precondition Failed"},
    {HttpResponse::RequestEntryTooLarge, "Request Entry Too Large"},
    {HttpResponse::RequestUriTooLarge, "Request URI Too Large"},
    {HttpResponse::UnsupportedMediaType, "Unsupported Media Type"},
    {HttpResponse::RequestedRangeNotSatisfiable, "Requested Range Not Satisfiable"},
    {HttpResponse::ExpectationFailed, "Expectation Failed"},
    {HttpResponse::InternalServerError, "Internal Server Error"},
    {HttpResponse::NotImplemented, "Not Implemented"},
    {HttpResponse::BadGateway, "Bad Gateway"},
    {HttpResponse::ServiceUnavailable, "Service Unavailable"},
    {HttpResponse::GatewayTimeout, "Gateway Timeout"},
    {HttpResponse::HttpVersionNotSupported, "HTTP Version Not Supported"}
};

HttpResponse::HttpResponse(IHttpClientHandler *pClientHandler)
    : m_pClientHandler(pClientHandler),
      m_majorVersion(1),
      m_minorVersion(1),
      m_status(Invalid),
      m_options(),
      m_cookies(),
      m_contentType(HttpContentType::TextHtml),
      m_data(),
      m_socketPtr(nullptr),
      m_sent(false)
{
}

HttpResponse::HttpResponse(Status s, IHttpClientHandler *pClientHandler)
    : m_pClientHandler(pClientHandler),
      m_majorVersion(1),
      m_minorVersion(1),
      m_status(s),
      m_options(),
      m_cookies(),
      m_contentType(HttpContentType::TextHtml),
      m_data(),
      m_socketPtr(nullptr),
      m_sent(false)
{
}

HttpResponse::HttpResponse(const HttpResponse &r)
    : m_pClientHandler(r.m_pClientHandler),
      m_majorVersion(r.m_majorVersion),
      m_minorVersion(r.m_minorVersion),
      m_status(r.m_status),
      m_options(r.m_options),
      m_cookies(r.m_cookies),
      m_contentType(HttpContentType::TextHtml),
      m_data(r.m_data),
      m_socketPtr(r.m_socketPtr),
      m_sent(false)
{
}

HttpResponse& HttpResponse::operator =(const HttpResponse &r)
{
    if (this != &r) {
        m_pClientHandler = r.m_pClientHandler,
        m_majorVersion = r.m_majorVersion;
        m_minorVersion = r.m_minorVersion;
        m_status = r.m_status;
        m_options = r.m_options;
        m_cookies = r.m_cookies;
        m_data = r.m_data;
        m_socketPtr = r.m_socketPtr;
        m_sent = r.m_sent;
    }
    return *this;
}

QTcpSocket* HttpResponse::socket() const
{
    return m_socketPtr.data();
}

void HttpResponse::setSocket(QTcpSocket *pSocket)
{
    m_socketPtr = pSocket;
}

void HttpResponse::send()
{
    Q_ASSERT(!m_socketPtr.isNull());
    if (m_socketPtr.isNull()) {
        return;
    }

    Q_ASSERT(!m_sent);
    if (m_sent) {
        return;
    }

    QString res = QString("HTTP/%1.%2 %3 %4\n")
            .arg(m_majorVersion)
            .arg(m_minorVersion)
            .arg(static_cast<int>(m_status))
            .arg(statusToString(m_status));

    QDataStream out(m_socketPtr.data());
    QByteArray ba = res.toLatin1();
    out.writeRawData(ba.constData(), ba.length());

    // Injecting content length
    if (!m_data.isEmpty()) {
        setOption(HttpOption::ContentLength, QString::number(m_data.length()));
    }

    sendOptions(out);
    sendCookies(out);

    out.writeRawData("\n", 1);

    // Sending data buffer
    if (!m_data.isEmpty()) {
        out.writeRawData(m_data.constData(), m_data.length());
    }

    m_sent = true;
}

void HttpResponse::sendData(const QByteArray &data)
{
    Q_ASSERT(!m_socketPtr.isNull());
    if (!m_socketPtr.isNull()) {
        m_socketPtr->write(data);
    }
}

void HttpResponse::finalize()
{
    if (m_pClientHandler != nullptr) {
        m_pClientHandler->finalizeResponse();
    }
}

QString HttpResponse::statusToString(const Status &s)
{
    return sStatusToReasonMap.value(s, "");
}

void HttpResponse::sendOptions(QDataStream &out)
{
    QMapIterator<QString, QVariant> i(m_options);
    while (i.hasNext()) {
        i.next();
        QString opt = QString("%1: %2\n")
                .arg(i.key())
                .arg(i.value().toString());
        QByteArray ba = opt.toLatin1();
        out.writeRawData(ba.constData(), ba.length());
    }
}

void HttpResponse::sendCookies(QDataStream &out)
{
    foreach (const QNetworkCookie &cookie, m_cookies.values()) {
        QByteArray rawCookie = cookie.toRawForm();

        out.writeRawData("Set-Cookie: ", 12);
        out.writeRawData(rawCookie.constData(), rawCookie.size());
        out.writeRawData("\n", 1);
    }
}

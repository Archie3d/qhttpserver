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

#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include <QVariant>
#include <QMultiMap>
#include <QTcpSocket>
#include <QNetworkCookie>
#include <QPointer>
#include "HttpServerApi.h"
#include "HttpContentType.h"
#include "IHttpClientHandler.h"

class HTTP_API HttpResponse
{
public:

    enum Status {
        Continue            = 100,
        SwitchingProtocols  = 101,
        Ok                  = 200,
        Created             = 201,
        Accepted            = 202,
        NonAuthoritativeInformation = 203,
        NoContent           = 204,
        ResetContent        = 205,
        PartialContent      = 206,
        MultipleChoices     = 300,
        MovedPermanently    = 301,
        Found               = 302,
        SeeOther            = 303,
        NotModified         = 304,
        UseProxy            = 305,
        TemporaryRedirect   = 307,
        BadRequest          = 400,
        Unauthorized        = 401,
        PaymentRequered     = 402,
        Forbidden           = 403,
        NotFound            = 404,
        MethodNotAllowed    = 405,
        NotAcceptable       = 406,
        ProxyAuthenticationRequired = 407,
        RequestTimeOut      = 408,
        Conflict            = 409,
        Gone                = 410,
        LengthRequired      = 411,
        PreconditionFailed  = 412,
        RequestEntryTooLarge    = 413,
        RequestUriTooLarge  = 414,
        UnsupportedMediaType    = 415,
        RequestedRangeNotSatisfiable    = 416,
        ExpectationFailed   = 417,
        InternalServerError = 500,
        NotImplemented      = 501,
        BadGateway          = 502,
        ServiceUnavailable  = 503,
        GatewayTimeout      = 504,
        HttpVersionNotSupported = 505,

        Processed           = 2,
        Unknown             = 1,
        Invalid             = 0
    };

    HttpResponse(IHttpClientHandler *pClientHandler = nullptr);
    HttpResponse(Status s, IHttpClientHandler *pClientHandler = nullptr);
    HttpResponse(const HttpResponse &r);
    HttpResponse& operator =(const HttpResponse &r);

    Status status() const { return m_status; }
    void setStatus(Status s) { m_status = s; }

    int majorVersion() const { return m_majorVersion; }
    void setMajorVersion(int v) { m_majorVersion = v; }
    int minorVersion() const { return m_minorVersion; }
    void setMinorVersion(int v) { m_minorVersion = v; }
    void setVersion(int major, int minor) { m_majorVersion = major; m_minorVersion = minor; }

    const QByteArray& constData() const { return m_data; }
    QByteArray& data() { return m_data; }
    void setData(const QByteArray &d) { m_data = d; }

    const QMultiMap<QString, QVariant>& options() const { return m_options; }
    QList<QNetworkCookie> cookies() const { return m_cookies.values(); }

    void setOption(const QString &name, const QVariant &value) { m_options.insert(name, value); }
    QVariant option(const QString &name) const { return m_options.value(name); }

    QTcpSocket* socket() const;
    void setSocket(QTcpSocket *pSocket);

    HttpContentType contentType() const { return m_contentType; }
    void setContentType(const HttpContentType &contentType) { m_contentType = contentType; }

    /**
     * Send this response.
     */
    void send();

    /**
     * Send some unstructured data.
     * @param data
     */
    void sendData(const QByteArray &data);

    /**
     * Tells whether the response has been already sent.
     * @return
     */
    bool isSent() const { return m_sent; }

    /**
     * Finalize the response.
     */
    void finalize();

    static QString statusToString(const Status &s);

private:

    void sendOptions(QDataStream &out);
    void sendCookies(QDataStream &out);

    IHttpClientHandler* m_pClientHandler;

    int m_majorVersion;
    int m_minorVersion;
    Status m_status;
    QMultiMap<QString, QVariant> m_options;
    QMap<QString, QNetworkCookie> m_cookies;
    HttpContentType m_contentType;
    QByteArray m_data;
    QPointer<QTcpSocket> m_socketPtr;
    bool m_sent;    ///< Flag to tell the response has been sent.
};

#endif // HTTPRESPONSE_H
